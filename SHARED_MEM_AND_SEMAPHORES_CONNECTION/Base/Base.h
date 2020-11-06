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

// BUFFER
#define BUF_SZ 4096
char BUFFER[BUF_SZ];

// SHARED MEMORY
#define SHMEM_SIZE BUF_SZ
int SHARED_MEM_ID;
char * SHMEM;

// SEMAPHORES
#define TOTAL_SEMS 6
#define START 0
#define MUTEX 1
#define FULL 2
#define ALIVE_R 3
#define ALIVE_W 4
#define DIE 5
int SEMS_ID;

