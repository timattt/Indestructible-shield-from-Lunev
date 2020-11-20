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
int wSEMOP5();
int wWRITE_FROM_BUFFER_TO_CONSOLE();
void wSHMDT();
void wSHMCTL();
void wSEMCTL();

// SEMAPHORES OPERATION:
#define wSEMOP_FAST(N) void wSEMOP ## N () {\
if (semop(SEMS_ID, wPART ## N, wP ## N ## SZ) == -1) {\
	ERROR_CONCAT("semop part ", #N);\
}\
}

#define wP1SZ 3
#define wP2SZ 3
#define wP3SZ 5
#define wP4SZ 1

#endif
