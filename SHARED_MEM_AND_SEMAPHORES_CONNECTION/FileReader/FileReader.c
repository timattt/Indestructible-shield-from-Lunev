#include "FileReader.h"

struct sembuf rPART1[rP1SZ] = {
		{FR_CUR,  0, IPC_NOWAIT},
		{FR_PRE,  0, IPC_NOWAIT},
		{FR_CUR, +1, SEM_UNDO}
};
struct sembuf rPART2[rP2SZ] = {
		{CW_CUR, -1, 0},
		{CW_CUR, +1, 0},
		{CW_PRE, +1, SEM_UNDO}
};
struct sembuf rPART3[rP3SZ] = {
		{FULL, +1, SEM_UNDO},
		{FULL, -1, 0}
};
struct sembuf rPART4[rP4SZ] = {
		{CW_CUR, -1, IPC_NOWAIT},
		{CW_CUR, +1, 0},
		{FULL,  0, 0},
		{MUTEX,  0, 0},
		{MUTEX, +1, SEM_UNDO}
};
struct sembuf rPART5[rP5SZ] = {
		{MUTEX , -1, SEM_UNDO},
		{FULL  , +1, 0}
};

void rDO_STEPS() {
	rOPEN_FILE();
	cGEN_KEY();
	cSHMGET_TRANSFER();
	cSHMAT_TRANSFER();
	cSEMGET_SEMAPHORS();
	rSEMOP1();
	rSEMOP2();
	rSEMOP3();
	rREAD_FROM_FILE_AND_WRITE_TO_TRANSFER_LOOP();
	rCLOSE_INPUT_FILE();
	cSHMDT();
}

void rOPEN_FILE() {
	if ((INPUT_FILE_FD = open(INPUT_FILE_NAME, O_RDONLY)) == -1) {
		ERROR("while opening file");
	}
}

rSEMOP_FAST(1)

rSEMOP_FAST(2)

rSEMOP_FAST(3)

rSEMOP_FAST(4)

void rREAD_FROM_FILE_AND_WRITE_TO_TRANSFER_LOOP() {
	int count = 0;
	while (1) {
		//===========================================
		rSEMOP4();
		//===========================================

		//===========================================
		memset(SHMEM, 0, SHMEM_SIZE);
		if ((count = read(INPUT_FILE_FD, SHMEM + sizeof(int), SHMEM_SIZE - sizeof(int))) < 0) {
			ERROR("while reading");
		}

		memcpy(SHMEM, &count, sizeof(int));
		//===========================================

		//===========================================
		rSEMOP5();
		//===========================================

		if (count == 0) {
			break;
		}
	}
}

rSEMOP_FAST(5)

void rCLOSE_INPUT_FILE() {
	if (close(INPUT_FILE_FD) == -1) {
		ERROR("while closing input file");
	}
}


