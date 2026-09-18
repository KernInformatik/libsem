#include "common.h"
#include "semaphore.h"

/**
 * @brief circular buffer structure
 * @details only the sharedMemory is shared. The semaphores need to be initialized, see "sempahore.h" library. The shm
 * data is capped to 2048 bytes (2KiB) but can be increased up to 4KiB
 */

/**
 * @brief This functions creates the circular buffer
 *
 * @details initializes the circularbuffer correspondingly for the server or the client.The caller must not
 * free() the returned circbuf struct, but must instead call closeCircularBuffer to free the resources. The caller shall
 * not use this function to write or read data from the circular buffer, for that see writeCircularBuffer()
 * readCircularBuffer()
 * @param isServer initializes the circular buffer corresponding to a server or a client
 * @return struct circBuff*
 */
void initializeCircularBuffer(struct shm *shm);

/**
 * @brief closes the circular buffer
 *
 * @param circBuff
 * @param isServer
 * @return int
 */
int closeCircularBuffer(bool isServer);

/**
 * @brief writes data into the circular buffer
 *
 * @param circBuff
 * @param value
 */
void writeCircularBuffer(int value);

/**
 * @brief reads from the circular buffer
 *
 * @param circBuff
 * @return int
 */
int readCircularBuffer();
