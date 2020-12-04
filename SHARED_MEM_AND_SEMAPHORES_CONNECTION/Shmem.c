#include "Shmem.h"

#define MAX_SEMS 5
#define BUF_SIZE 4096

#define PREPARE_SEMS_BUF struct sembuf sems_buf[MAX_SEMS] = { 0 }; int curr_semop = 0;
#define FILL_SEMBUF(NUM, OP, FLAG) sems_buf[curr_semop].sem_num = NUM;\
	sems_buf[curr_semop].sem_op = OP;\
	sems_buf[curr_semop].sem_flg = FLAG;\
	curr_semop++;
#define SEMOP if (semop(semid, sems_buf, curr_semop) < 0) {ERROR("semop");} curr_semop = 0;
#define SET_SEM_VAL(SEM, VAL) if (semctl(semid, SEM, SETVAL, VAL) < 0) {ERROR("while semctl");}
#define GET_SEM_VAL(SEM) semctl(semid, SEM, GETVAL)

void common(int * semid, int * shmid, char ** shmem) {
	//---------------
	// GEN KEY
	int key = ftok("/home/timattt/.profile", 1);
	if (key < 0) {
		ERROR("while ftok");
	}

	//---------------
	// semget
	*semid = 0;
	if ((*semid = semget(key, MAX_SEMS, IPC_CREAT | 0666)) < 0) {
		ERROR("while semget");
	}

	//---------------
	// shmget
	*shmid = 0;
	if ((*shmid = shmget(key, BUF_SIZE, IPC_CREAT | 0666)) < 0) {
		ERROR("while shmget");
	}

	//---------------
	// shmat
	*shmem = (char*) shmat(*shmid, NULL, 0);
	if (*shmem == (char*)(-1)) {
		ERROR("while shmat");
	}
}

void shmem_fileReader(char * fileName, int semid, char * shmem) {
	//---------------
	// open file
	int fd = open(fileName, O_RDONLY);

	//---------------

	PREPARE_SEMS_BUF;

	//---------------
	// Critical section I
	// Conflict between readers for shared memory
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

	FILL_SEMBUF(0, 0, 0);
	FILL_SEMBUF(0, 1, SEM_UNDO);
	SEMOP;

	// till death
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//---------------

	FILL_SEMBUF(4, 0, 0);
	SEMOP;

	//---------------
	// Critical section II
	// Conflict between reader and writer in one pair for value of sem 3.
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

	SET_SEM_VAL(3, 1);

	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//---------------

	FILL_SEMBUF(0, 1, SEM_UNDO);
	FILL_SEMBUF(3, -1, SEM_UNDO);
	SEMOP;

	//---------------

	FILL_SEMBUF(1, -2, 0);
	FILL_SEMBUF(1, 2, 0);
	FILL_SEMBUF(4, 1, SEM_UNDO);
	SEMOP;

	//---------------

	int sz = 0;
	do {
		//---------------
		// Critical section III
		// Conflict between reader and writer for shmem
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

		FILL_SEMBUF(2, -1, 0);
		SEMOP;

		//---------------

		if (GET_SEM_VAL(4) != 2) {
			break;
		}

		//---------------

		memset(shmem, 0, BUF_SIZE);
		sz = read(fd, shmem + sizeof(int), BUF_SIZE - sizeof(int));
		if (sz < 0) {
			ERROR("while reading from file");
		}
		*shmem = sz;

		//---------------

		FILL_SEMBUF(3, 1, 0);
		SEMOP;

		//---------------
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	} while (sz > 0);

}

void shmem_consoleWriter(int semid, char * shmem) {
	PREPARE_SEMS_BUF;

	//---------------
	// Critical section IV
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	// Conflict between writers for shared memory

	FILL_SEMBUF(1, 0, 0);
	FILL_SEMBUF(1, 1, SEM_UNDO);
	SEMOP;

	// till death
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//---------------

	FILL_SEMBUF(4, 0, 0);
	SEMOP;

	//---------------
	// V
	// Conflict between reader and writer in one pair for value of sem 2
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

	SET_SEM_VAL(2, 2);

	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//---------------

	FILL_SEMBUF(1, 1, SEM_UNDO);
	FILL_SEMBUF(2, -1, SEM_UNDO);
	SEMOP;

	//---------------

	FILL_SEMBUF(0, -2, 0);
	FILL_SEMBUF(0, 2, 0);
	FILL_SEMBUF(4, 1, SEM_UNDO);
	SEMOP;

	//---------------

	int sz = 0;
	do {
		//---------------
		// Critical section VI
		// Conflict between reader and writer for shmem
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

		FILL_SEMBUF(3, -1, 0);
		SEMOP;

		//---------------

		if (GET_SEM_VAL(4) != 2 && *(int*)shmem == BUF_SIZE - sizeof(int)) {
			break;
		}

		sz = *(int*)shmem;

		write(STDOUT_FILENO, shmem + sizeof(int), BUF_SIZE - sizeof(int));

		//---------------

		FILL_SEMBUF(2, 1, 0);
		SEMOP;

		//---------------
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	} while (sz > 0);
}

int Task3(int argc, char * argv[]) {
	if (argc < 2) {
		ERROR("too fex arguments");
	}

	int semid = 0;
	int shmid = 0;
	char * shmem = NULL;

	common(&semid, &shmid, &shmem);

	if (argv[1][0] == 'r') {
		if (argc < 3) {
			ERROR("too fex arguments");
		}
		shmem_fileReader(argv[2], semid, shmem);
	}
	if (argv[1][0] == 'w') {
		shmem_consoleWriter(semid, shmem);
	}

	return 0;
}
