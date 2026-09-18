#include "circularbuffer.h"
#include "semaphore.h"

struct circBuff *initializeCircularBuffer(bool isServer)
{
    struct circBuff *rv = (struct circBuff *)malloc(sizeof(struct circBuff));
    if (isServer)
    {
        rv->sharedMemory->alive = true;
        rv->sharedMemory = sharedMemory_Server(&rv->shmfd);
        rv->free = initializeSemaphore_Server(FREE_SPACE_SEMAPHORE, FREE_SPACE_SEMAPHORE_SIZE);
        rv->write = initializeSemaphore_Server(WRITE_SPACE_SEMAPHORE, WRITE_SPACE_SEMAPHORE_SIZE);
        rv->used = initializeSemaphore_Server(USED_SPACE_SEMAPHORE, USED_SPACE_SEMAPHORE_SIZE);
    }
    else
    {
        rv->sharedMemory = sharedMemory_Client(&rv->shmfd);
        rv->free = initializeSemaphore_Client(FREE_SPACE_SEMAPHORE);
        rv->write = initializeSemaphore_Client(WRITE_SPACE_SEMAPHORE);
        rv->used = initializeSemaphore_Client(USED_SPACE_SEMAPHORE);
    }

    return rv;
}

int closeCircularBuffer(struct circBuff *circBuff, bool isServer)
{
    if (isServer)
    {
        circBuff->sharedMemory->alive = false;
        cleanSharedMemory_Server(circBuff->sharedMemory, circBuff->shmfd);
        cleanSemaphore_Server(circBuff->free, FREE_SPACE_SEMAPHORE);
        cleanSemaphore_Server(circBuff->used, USED_SPACE_SEMAPHORE);
        cleanSemaphore_Server(circBuff->write, WRITE_SPACE_SEMAPHORE);
    }
    else
    {
        cleanSharedMemory_Client(circBuff->sharedMemory, circBuff->shmfd);
        cleanSemaphore_Client(circBuff->free);
        cleanSemaphore_Client(circBuff->used);
        cleanSemaphore_Client(circBuff->write);
    }
    free(circBuff);
}

void writeCircularBuffer(struct circBuff *circBuff, int value)
{
    if (sem_wait(circBuff->write) == -1)
    {
        return;
    }

    if (sem_wait(circBuff->free) == -1)
    {
        sem_post(circBuff->write);
        return;
    }

    struct shm *shm = circBuff->sharedMemory;
    shm->data[shm->writehead] = value;
    circBuff->sharedMemory->writehead++;
    circBuff->sharedMemory->writehead %= MAX_BUFF_SIZE;

    sem_post(circBuff->used);
    sem_post(circBuff->write);
    return;
}

int readCircularBuffer(struct circBuff *circBuff)
{
    if (sem_wait(circBuff->used) == -1)
        return -1;

    struct shm *shm = circBuff->sharedMemory;
    int rv = (int)shm->data[shm->readhead];
    shm->readhead = (shm->readhead + 1) % MAX_BUFF_SIZE;

    sem_post(circBuff->free);
    return rv;
}
