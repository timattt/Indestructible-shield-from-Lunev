#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "FIFO_CONNECTION/Fifo.h"
#include "MESSAGES/Main.h"
#include "PROXY/Proxy.h"
#include "SHARED_MEM_AND_SEMAPHORES_CONNECTION/Shmem.h"
#include "UTILS/Utils.h"

#define DEFAULT_PROG 5

int main(int argc, char *argv[]) {

	char buf[3] = { 0 };
	if (mkfifo("fifo", 666) < 0) {
		ERROR("mkfifo");
	}

	int fd = 0;
	if ((fd = open("fifo", O_RDWR)) < 0) {
		ERROR("open 1");
	}

	if (write(fd, "aaa", 3) < 0) {
		ERROR("write 1");
	}

	if ((fd = open("fifo", O_RDONLY | O_NONBLOCK)) < 0) {
		ERROR("open 2");
	}

	int count = 0;
	if ((count = read(fd, buf, 3)) < 0) {
		ERROR("read");
	}

	if (write(1, buf, count) < 0) {
		ERROR("write 2");
	}


	remove("fifo");

	return 0;
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
	case 5:
		Task5(argc, argv);
		break;
	default:
		ERROR("while starting progs, strange prog number");
		break;
	}

	return 0;
}
