#include "FileReader.h"

struct sembuf rPART1[rP1SZ] = {
		{START, 0, IPC_NOWAIT},
		{START, 1, 0},
		{MUTEX, 1, 0},
		{ALIVE_R, 1, 0},
		{ALIVE_W, 1, 0},
		{DIE, 2, 0}
};
struct sembuf rPART2[rP2SZ] = {
		{DIE, -2, IPC_NOWAIT},
		{DIE, 2, 0},
		{ALIVE_W, -1, SEM_UNDO},
		{FULL, 1, SEM_UNDO},
		{FULL, -1, 0}
};
struct sembuf rPART3[rP3SZ] = {
		{ALIVE_R, 0, 0},
		{DIE, -1, SEM_UNDO}
};
struct sembuf rPART4[rP4SZ] = {
		{DIE, 0, 0}
};
struct sembuf rPART5[rP5SZ] = {
		{DIE, 0, IPC_NOWAIT}
};
struct sembuf rPART6[rP6SZ] = {
		{DIE, 0, IPC_NOWAIT},
		{FULL, 0, 0},
		{MUTEX, -1, SEM_UNDO}
};
struct sembuf rPART7[rP7SZ] = {
		{FULL, 1, 0},
		{MUTEX, 1, SEM_UNDO}
};
struct sembuf rPART8[rP8SZ] = {
		{FULL, 0, 0},
		{DIE, 0, IPC_NOWAIT}
};

void rDO_STEPS() {
	rOPEN_FILE();
	cGEN_KEY();
	cSHMGET_TRANSFER();
	cSHMAT_TRANSFER(); // шмат сала :)
	cSEMGET_SEMAPHORS();
	rSEMOP1_NO_EAGAIN();
	rSEMOP2();
	rSEMOP3();
	rSEMOP4();
	rREAD_FROM_FILE_AND_WRITE_TO_TRANSFER_LOOP();
	rCLOSE_INPUT_FILE();
	rSEMOP8();
	cSHMDT();
}

void rOPEN_FILE() {
	if ((INPUT_FILE_FD = open(INPUT_FILE_NAME, O_RDONLY)) == -1) {
		ERROR("while opening file");
	}
}

rSEMOP_NO_EAGAIN_FAST(1)

rSEMOP_FAST(2)

rSEMOP_FAST(3)

rSEMOP_FAST(4)

void rREAD_FROM_FILE_AND_WRITE_TO_TRANSFER_LOOP() {
	int count = 0;
	while (1) {
		if ((count = read(INPUT_FILE_FD, BUFFER, BUF_SZ)) < 0) {
			ERROR("while reading");
		}

		rSEMOP5();
		rSEMOP6();

		memset(BUFFER, 0, BUF_SZ);
		memcpy(SHMEM, BUFFER, BUF_SZ);

		rSEMOP7();

		if (count == 0) {
			break;
		}
	}
}

rSEMOP_FAST(5)

rSEMOP_FAST(6)

rSEMOP_FAST(7)

void rCLOSE_INPUT_FILE() {
	if (close(INPUT_FILE_FD) == -1) {
		ERROR("while closing input file");
	}
}

rSEMOP_FAST(8)


