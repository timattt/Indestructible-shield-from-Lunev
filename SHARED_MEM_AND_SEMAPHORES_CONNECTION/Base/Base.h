// INCLUDES
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>

// COMMON FUNCTIONS
void cGEN_KEY();
void cSHMGET_TRANSFER();
void cSHMAT_TRANSFER();
void cSEMGET_SEMAPHORS();
void cSHMDT();

// COMMON DATA
int KEY;

// SHARED MEMORY
#define SHMEM_SIZE 4096
int SHARED_MEM_ID;
char * SHMEM;

// SEMAPHORES
#define TOTAL_SEMS 8
#define CSM_CUR 0
#define CSM_PRE 1
#define PDC_CUR 2
#define PDC_PRE 3
#define MUTEX 4
#define FULL 5
#define CTL 6
#define SEM_NUM 7
int SEMS_ID;

