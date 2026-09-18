#include "semaphore.h"

/*STATIC HELPER FUNCTIONS*/

static void
error_exit(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

static void
error_exit_failure(int val, const char *msg)
{
	if (val == -1)
		error_exit(msg);
}

static struct shm *
openSharedMemory(int *shmfd, bool isServer)
{
	int oflag = isServer ? SHM_OFLAG_SERVER : SHM_OFLAG_CLIENT;
	const char *openErr =
	    isServer ? OPENING_SHM_ERROR_SERVER : OPENING_SHM_ERROR_CLIENT;

	*shmfd = shm_open(SHM_NAME, oflag, SHM_MODE);
	error_exit_failure(*shmfd, openErr);

	if (isServer)
		error_exit_failure(ftruncate(*shmfd, sizeof(struct shm)),
		    TRUNCATING_SHM_ERROR);

	struct shm *rv = mmap(NULL, sizeof(struct shm), MMP_PROT_FLAGS,
	    MMP_FLAGS, *shmfd, MMP_OFFSET);
	if (rv == MAP_FAILED)
		error_exit(MAPPING_SHM_ERROR);

	if (isServer)
		memset(rv->data, 0, MAX_BUFF_SIZE * sizeof(int));
	return rv;
}

static void
closeSharedMemory(struct shm *shm, int *shmfd, bool isServer)
{
	error_exit_failure(munmap(shm, sizeof(*shm)), UNMAPPING_SHM_ERROR);
	error_exit_failure(close(*shmfd), CLOSING_SHM_ERROR);

	if (isServer)
		error_exit_failure(shm_unlink(SHM_NAME), UNLINKING_SHM_ERROR);
}

static sem_t *
openSemaphore(const char *semaphore_name, bool isServer,
    unsigned int initial_value)
{
	sem_t *rv = isServer ? sem_open(semaphore_name, OPEN_SEM_MODE_SERVER,
	    OPEN_SEM_MODE, initial_value) :
	    sem_open(semaphore_name, OPEN_SEM_MODE_CLIENT);

	if (rv == SEM_FAILED)
		error_exit(OPENING_SEM_ERROR);

	return rv;
}

static void
closeSemaphore(sem_t *semaphore, const char *semaphore_name,
    bool isServer)
{
	error_exit_failure(sem_close(semaphore), CLOSING_SEM_ERROR);

	if (isServer)
		error_exit_failure(sem_unlink(semaphore_name),
		    UNLINKING_SEM_ERROR);
}

/*END STATIC HELPER FUNCTIONS*/

/*API REGION */

struct shm *
sharedMemory_Server(int *shmfd)
{
	return openSharedMemory(shmfd, true);
}

struct shm *
sharedMemory_Client(int *shmfd)
{
	return openSharedMemory(shmfd, false);
}

void
cleanSharedMemory_Server(struct shm *shm, int shmfd)
{
	closeSharedMemory(shm, &shmfd, true);
}

void
cleanSharedMemory_Client(struct shm *shm, int shmfd)
{
	closeSharedMemory(shm, &shmfd, false);
}

sem_t *
initializeSemaphore_Server(const char *sem_name,
    unsigned int init_value)
{
	return openSemaphore(sem_name, true, init_value);
}

sem_t *
initializeSemaphore_Client(const char *sem_name)
{
	return openSemaphore(sem_name, false, 0);
}

void
cleanSemaphore_Server(sem_t *semaphore, const char *sem_name)
{
	closeSemaphore(semaphore, sem_name, true);
}

void
cleanSemaphore_Client(sem_t *semaphore)
{
	closeSemaphore(semaphore, NULL, false);
}

/*END API REGION*/
