#ifndef FILE_READER_SHMEM
#define FILE_READER_SHMEM

// INCLUDES
#include <sys/msg.h>
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
void rDO_STEPS();
void rOPEN_FILE();
void rSEMOP1_NO_EAGAIN();
void rSEMOP2();
void rSEMOP3();
void rSEMOP4();
void rREAD_FROM_FILE_AND_WRITE_TO_TRANSFER_LOOP();
/**/void rSEMOP5();
/**/void rSEMOP6();
/**/void rSEMOP7();
void rCLOSE_INPUT_FILE();
void rSEMOP8();

// SEMAPHORES OPERATION:
#define rSEMOP_FAST(N) void rSEMOP ## N () {\
if (semop(SEMS_ID, rPART ## N, rP ## N ## SZ) == -1) {\
	ERROR_CONCAT("semop part ", #N);\
}\
}

#define rSEMOP_NO_EAGAIN_FAST(N) void rSEMOP ## N ## _NO_EAGAIN() {\
if (semop(SEMS_ID, rPART ## N, rP ## N ## SZ) == -1 && errno != EAGAIN) {\
	ERROR_CONCAT("semop part ", #N);\
}\
}

#define rP1SZ 2
#define rP2SZ 5
#define rP3SZ 2
#define rP4SZ 1
#define rP5SZ 1
#define rP6SZ 3
#define rP7SZ 2
#define rP8SZ 2

// DATA
// INPUT FILE
char * INPUT_FILE_NAME;
int INPUT_FILE_FD;

#endif
