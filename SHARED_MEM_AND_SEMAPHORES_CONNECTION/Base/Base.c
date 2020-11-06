#include "Base.h"

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "../../UTILS/Utils.h"

void cGEN_KEY() {
	KEY = ftok("/home/timattt/.profile", 1);
	if (KEY < 0) {
		ERROR("while ftok");
	}
}

void cSHMGET_TRANSFER() {
	if ((SHARED_MEM_ID = shmget(KEY, SHMEM_SIZE, IPC_CREAT | 0600)) < 0) {
		ERROR("while allocating shared memory");
	}
}

void cSHMAT_TRANSFER() {
	if ((SHMEM = (char*) shmat(SHARED_MEM_ID, NULL, 0)) == (char*)(-1)) {
		ERROR("while attaching shared memory");
	}
}

void cSEMGET_SEMAPHORS() {
	if ((SEMS_ID = semget(KEY, TOTAL_SEMS, IPC_CREAT | 0600)) < 0) {
		ERROR("while getting sempahores set");
	}
}

void cSHMDT() {
	if (shmdt(SHMEM) == -1) {
		ERROR("while detaching shared memory");
	}
}

