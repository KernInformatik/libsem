/**
 * @file common.h
 * @author Kenrkraftwerk (kernkraftdev@hotmail.com)
 * @brief a library for macros and libraries
 * @version 0.1
 * @date 2026-09-15
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef COMMON
#define COMMON

/*Checks and balances(?) */
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <unistd.h>

/*IPC Process communication and shared memory */
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

/*Standard libraries */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*String libary */
#include <string.h>
#include <strings.h>

/*Misc*/
#include <regex.h>
#include <time.h>

#define error(msg)                                                                                                     \
    do                                                                                                                 \
    {                                                                                                                  \
        perror(msg);                                                                                                   \
        printf("Error (%d): %s\n", errno, strerror(errno));                                                            \
        exit(EXIT_FAILURE);                                                                                            \
    } while (true)

/*Put datastructures here if needed, else create for datastructure a header */
#endif
