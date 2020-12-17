#include "Utils.h"

#include <asm-generic/errno-base.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

void ERROR(const char * mes) {
	printf("Error with code %d, %s\nPrinting message:\n", errno, mes);
	perror("");
	exit(-1);
}

void ERROR_CONCAT(const char * mes, const char * mes1) {
	printf("Error with code %d, %s%s\nPrinting message:\n", errno, mes, mes1);
	perror("");
	exit(-1);
}

int SAME_FD_SAME_FILES(int fd1, int fd2) {
	errno = 0;
	struct stat stat1, stat2;
	if (fstat(fd1, &stat1) < 0)
		return -1;
	if (fstat(fd2, &stat2) < 0)
		return -1;
	return (stat1.st_dev == stat2.st_dev) && (stat1.st_ino == stat2.st_ino);
}

int GET_MAX_POSIBLE_DESCRIPTORS_COUNT() {
	FILE* fd = popen("ulimit -Hn", "r");
	if (fd == NULL) {
		ERROR("get max");
	}
	int lim = 0;
	fscanf(fd, "%d", &lim);
	pclose(fd);
	return lim;
}

int COUNT_FD(char * fileName) {
	int fd = open(fileName, O_RDONLY | O_NONBLOCK);
	if (fd < 0) {
		ERROR("could not open file to count file descriptors");
	}

	int max = GET_MAX_POSIBLE_DESCRIPTORS_COUNT();
	int result = 0;
	for (int i = 3; i < max; i++) {
		if (i == fd) {
			continue;
		}
		if (SAME_FD_SAME_FILES(fd, i) == 1) {
			result++;
		}
		if (errno != 0 && errno != EBADF) {
			ERROR("while counting");
			return result;
		}
	}

	return result;
}

long STRING_TO_INT(char *str) {
	long input = 0;
	char *strptr = str, *endptr = NULL;
	input = strtol(strptr, &endptr, 10);

	if (endptr == strptr || *endptr != '\0') {
		ERROR("Wront input string");
		return -3;
	}

	if (input <= 0) {
		ERROR("The number must be greater then 0");
		return -4;
	}

	if (errno == ERANGE && (input == LONG_MAX || input == LONG_MIN)) {
		ERROR("Out of range");
		return -5;
	}

	return input;
}
