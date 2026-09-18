/**
 * @file circularbuffer.c
 * @author your name (you@domain.com)
 * @brief  circular buffer implementation
 * @details prerequisites. The caller of this library should have initialized the corresponding semaphores and has
 * already initialized a shared memory object. Since a circular buffer is nothing more than ana array with positional
 * read/write heads, limited via the modulo operator
 * @version 0.1
 * @date 2026-09-18
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "circularbuffer.h"
#include "semaphore.h"

static struct shm *circularBuffer;

void initializeCircularBuffer(struct shm *shm)
{
    circularBuffer = shm;
    circularBuffer->writehead = 0, circularBuffer->readhead = 0;
}

int closeCircularBuffer(bool isServer)
{
    free(circularBuffer);
    printf("Please close the SHM object cleanSharedMemory_*\n");
    return 0;
}

void writeCircularBuffer(int value)
{
    sem_wait(circularBuffer->free);
    circularBuffer->data[circularBuffer->writehead] = value;
    sem_post(circularBuffer->used);
    circularBuffer->writehead++;
    circularBuffer->writehead %= MAX_BUFF_SIZE;
}

int readCircularBuffer()
{
    sem_wait(circularBuffer->used);
    int rv = circularBuffer->data[circularBuffer->readhead];
    sem_post(circularBuffer->free);
    circularBuffer->readhead = (circularBuffer->readhead++) % MAX_BUFF_SIZE;
    return rv;
}
