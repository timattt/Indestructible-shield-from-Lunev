#include "ConsoleWriter.h"

struct sembuf wPART1[wP1SZ] = {
		{CSM_CUR,  0, IPC_NOWAIT},
		{CSM_PRE,  0, IPC_NOWAIT},
		{CSM_CUR, +1, SEM_UNDO}
};
struct sembuf wPART2[wP2SZ] = {
		{PDC_CUR, -1, 0},
		{PDC_CUR, +1, 0},
		{PDC_PRE, +1, SEM_UNDO}

};
struct sembuf wPART3[wP3SZ] = {
		{PDC_CUR, -1, IPC_NOWAIT},
		{PDC_CUR, +1, 0},
		{MUTEX  ,  0, 0},
		{MUTEX  , +1, SEM_UNDO},
		{FULL	, -1, 0}
};
struct sembuf wPART4[wP4SZ] = {
		{MUTEX, -1, SEM_UNDO}
};

void wDO_STEPS() {
	cGEN_KEY();
	cSHMGET_TRANSFER();
	cSHMAT_TRANSFER();
	cSEMGET_SEMAPHORS();
	wSEMOP1();
	wSEMOP2();
	wREAD_WRITE_LOOP();
	cSHMDT();
	wSHMCTL();
	wSEMCTL();
}

wSEMOP_FAST(1)

wSEMOP_FAST(2)

wSEMOP_FAST(3)

wSEMOP_FAST(4)

void wREAD_WRITE_LOOP() {
	int count = 0;
	while (1) {
		//===========================================
		wSEMOP3();
		//===========================================

		//===========================================
		memcpy(&count, SHMEM, sizeof(int));

		int ret = 0;
		if ((ret = write(STDOUT_FILENO, SHMEM + sizeof(int), count)) < 0) {
			ERROR("while writing to console");
		}
		//===========================================

		//===========================================
		wSEMOP4();
		//===========================================

		//===========================================
		if (ret == 0) {
			break;
		}
		//===========================================
	}
}

void wSHMCTL() {
	if (shmctl(SHARED_MEM_ID, IPC_RMID, NULL) < 0) {
		ERROR("while shmctl");
	}
}

void wSEMCTL() {
	if (semctl(SEMS_ID, 0, IPC_RMID) < 0) {
		ERROR("while semctl");
	}
}
