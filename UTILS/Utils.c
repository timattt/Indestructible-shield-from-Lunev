#include "Utils.h"

#include <asm-generic/errno-base.h>
#include <sys/stat.h>

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
	struct stat stat1, stat2;
	if (fstat(fd1, &stat1) < 0)
		return -1;
	if (fstat(fd2, &stat2) < 0)
		return -1;
	return (stat1.st_dev == stat2.st_dev) && (stat1.st_ino == stat2.st_ino);
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
