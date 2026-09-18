# libsem

A small C library for POSIX shared memory + semaphore based IPC between one server process and multiple client processes, plus a circular buffer built on top of it.

## What's in here

- `lib/semaphore.h` / `lib/semaphore.c` — wraps `shm_open(3)`, `mmap(2)`, and `sem_open(3)` into create/connect/destroy/disconnect functions. One process is the "server" (creates the shared memory and semaphores), any number of processes can be "clients" (connect to what the server created).
- `lib/circularbuffer.h` / `lib/circularbuffer.c` — a circular buffer built on top of the shared memory + semaphores, with `free`/`used`/`write` semaphores for synchronization.
- `lib/common.h` — shared includes and an `error(msg)` macro that prints `perror` + `errno` and exits.

## Requirements

- Linux (or another POSIX system with `shm_open`/`sem_open`)
- GCC or another C compiler
- Link against `-lpthread` / `-lrt` as needed by your libc for POSIX semaphores

## Building

There's a Makefile that builds a static library, `build/libsem.a`.

```sh
make            # default flags: -std=c99 -pedantic -g -Wall
make verbose    # adds -Wextra -Wwrite-strings -Wconversion -Wformat=2 -Warray-bounds -Wstack-protector -Wshadow
make asan       # default flags + AddressSanitizer/UBSan
make harden     # default flags + compile-time hardening (stack protector, fortify source, etc.)
make clean
```

Then link your program against it:

```sh
gcc your_app.c build/libsem.a -o your_app -lpthread -pthread -lrt
```

If you used `make harden`, also add the link-time hardening flags it prints at the end of the build (`-pie -Wl,-z,relro,-z,now -Wl,-z,noexecstack`), since those only matter at the final link step, not when building the static library itself.

## Usage

### Shared memory + semaphores directly

Server side:

```c
#include "semaphore.h"

int shmfd;
struct shm *shm = sharedMemory_Server(&shmfd);

sem_t *free_sem = initializeSemaphore_Server(FREE_SPACE_SEMAPHORE, FREE_SPACE_SEMAPHORE_SIZE);
sem_t *used_sem = initializeSemaphore_Server(USED_SPACE_SEMAPHORE, USED_SPACE_SEMAPHORE_SIZE);
sem_t *write_sem = initializeSemaphore_Server(WRITE_SPACE_SEMAPHORE, WRITE_SPACE_SEMAPHORE_SIZE);

// ... use shm ...

cleanSemaphore_Server(free_sem, FREE_SPACE_SEMAPHORE);
cleanSemaphore_Server(used_sem, USED_SPACE_SEMAPHORE);
cleanSemaphore_Server(write_sem, WRITE_SPACE_SEMAPHORE);
cleanSharedMemory_Server(shm, shmfd);
```

Client side:

```c
#include "semaphore.h"

int shmfd;
struct shm *shm = sharedMemory_Client(&shmfd);
sem_t *free_sem = initializeSemaphore_Client(FREE_SPACE_SEMAPHORE);

// ... use shm ...

cleanSemaphore_Client(free_sem);
cleanSharedMemory_Client(shm, shmfd);
```

The server must call its `*_create`/`*_Server` functions before any client calls the matching `*_Client` function, since the server owns creation of the underlying resources. There must be exactly one server; there can be many clients.

### Circular buffer

```c
#include "circularbuffer.h"

// server process
struct circBuff *cb = initializeCircularBuffer(true);
writeCircularBuffer(cb, 42);
closeCircularBuffer(cb, true);

// client process
struct circBuff *cb = initializeCircularBuffer(false);
int value = readCircularBuffer(cb);
closeCircularBuffer(cb, false);
```

`writeCircularBuffer` and `readCircularBuffer` block on the underlying semaphores as needed (write waits for free space, read waits for used space).

## Notes / limitations

- Buffer capacity is fixed at `MAX_BUFF_SIZE` (2048 elements), defined in `semaphore.h`.
- Shared memory and semaphore names are hardcoded (`SHM_NAME`, `FREE_SPACE_SEMAPHORE`, etc. in `semaphore.h`), so only one instance of this IPC setup can exist on a system at a time.
- Server-side create calls use `O_EXCL`, so creating twice without cleaning up the previous run will fail — make sure the server unlinks its resources on exit.
- On failure, most functions call `error()`, which prints the error and calls `exit(EXIT_FAILURE)`. This is a library for controlled processes, not one that fails gracefully.

## License

AGPL-3.0. See `LICENSE`.
