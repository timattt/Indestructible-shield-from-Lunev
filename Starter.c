#include <stdio.h>

#include "FIFO_CONNECTION/Main.h"
#include "MESSAGES/Main.h"
#include "SHARED_MEM_AND_SEMAPHORES_CONNECTION/Shmem.h"
#include "UTILS/Utils.h"

#define DEFAULT_PROG 3

int main(int argc, char *argv[]) {
	int progn = DEFAULT_PROG;
	if (argc > 2 && argv[1][0] == 'p') {
		progn = STRING_TO_INT(argv[2]);
		argc -= 2;
		argv += 2;
	}

	printf("Running %d program!\n", progn);

	switch (progn) {
	case 1:
		Task1(argc, argv);
		break;
	case 2:
		Task2(argc, argv);
		break;
	case 3:
		Task3(argc, argv);
		break;
	default:
		ERROR("while starting progs, strange prog number");
		break;
	}

	return 0;
}
