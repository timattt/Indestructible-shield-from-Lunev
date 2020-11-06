#ifndef CONSOLE_WRITER_SHMEM
#define CONSOLE_WRITER_SHMEM

// INCLUDES
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>

#include "../../UTILS/Utils.h"

#include "../Base/Base.h"

// STEPS
void wDO_STEPS();
void wSEMOP1();
void wSEMOP2();
void wSEMOP3();
void wSEMOP4();
void wREAD_WRITE_LOOP();
int wSEMOP5_NO_EAGAIN();
int wSEMOP6_NO_EAGAIN();
int wWRITE_FROM_BUFFER_TO_CONSOLE();
void wSEMOP7();
void wSHMDT();
void wSHMCTL();
void wSEMCTL();

// SEMAPHORES OPERATION:
#define wSEMOP_FAST(N) void wSEMOP ## N () {\
if (semop(SEMS_ID, wPART ## N, wP ## N ## SZ) == -1) {\
	ERROR_CONCAT("semop part ", #N);\
}\
}

#define wSEMOP_NO_EAGAIN_FAST(N) int wSEMOP ## N ## _NO_EAGAIN() {\
int ret = 0;\
if ((ret = semop(SEMS_ID, wPART ## N, wP ## N ## SZ)) == -1 && errno != EAGAIN) {\
	ERROR_CONCAT("semop part ", #N);\
}return ret;\
}

#define wP1SZ 2
#define wP2SZ 3
#define wP3SZ 2
#define wP4SZ 1
#define wP5SZ 1
#define wP6SZ 3
#define wP7SZ 1

#endif
