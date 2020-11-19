#include "ConsoleWriter.h"

struct sembuf wPART1[wP1SZ] = {
		{START, 0, IPC_NOWAIT},
		{START, 1, 0},
		{MUTEX, 1, 0},
		{ALIVE_R, 1, 0},
		{ALIVE_W, 1, 0},
		{DIE, 2, 0}
};
struct sembuf wPART2[wP2SZ] = {
		{DIE, -2, IPC_NOWAIT},
		{DIE, 2, 0},
		{ALIVE_R, -1, SEM_UNDO}
};
struct sembuf wPART3[wP3SZ] = {
		{ALIVE_W, 0, 0},
		{DIE, -1, SEM_UNDO}
};
struct sembuf wPART4[wP4SZ] = {
		{DIE, 0, 0}
};
struct sembuf wPART5[wP5SZ] = {
		{DIE, 0, IPC_NOWAIT}
};
struct sembuf wPART6[wP6SZ] = {
		{DIE, 0, IPC_NOWAIT},
		{FULL, -1, 0},
		{MUTEX, -1, SEM_UNDO}
};
struct sembuf wPART7[wP7SZ] = {
		{MUTEX, 1, SEM_UNDO}
};

void wDO_STEPS() {
	cGEN_KEY();
	cSHMGET_TRANSFER();
	cSHMAT_TRANSFER();
	cSEMGET_SEMAPHORS();
	wSEMOP1();
	wSEMOP2();
	wSEMOP3();
	wSEMOP4();
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
	while (1) {
		//===========================================
		if (wSEMOP5_NO_EAGAIN() < 0) {
			break;
		}
		//===========================================


		//===========================================
		if (wSEMOP6_NO_EAGAIN() < 0) {
			break;
		}
		//===========================================


		//===========================================
		memcpy(BUFFER, SHMEM, BUF_SZ);
		//===========================================


		//===========================================
		wSEMOP7();
		//===========================================


		//===========================================
		if (wWRITE_FROM_BUFFER_TO_CONSOLE() == 0) {
			break;
		}
		//===========================================
	}
 }

wSEMOP_NO_EAGAIN_FAST(5)

wSEMOP_NO_EAGAIN_FAST(6)

wSEMOP_FAST(7)

int wWRITE_FROM_BUFFER_TO_CONSOLE() {
	int ret = 0;
	if ((ret = write(STDOUT_FILENO, BUFFER, BUF_SZ)) < 0) {
		ERROR("while writing to console");
	}
	return ret;
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
