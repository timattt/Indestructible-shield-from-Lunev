#include "Base.h"

void ERROR(const char * mes) {
	printf("Error with code %d, %s\n", errno, mes);
	exit(-1);
}

void MAKE_NAME(int key, char * dest) {
	for (int i = 0; i < NAME_LENGTH; i++) {
		dest[i] = 'a' + (key % ALPHABET_SIZE);
		key /= ALPHABET_SIZE;
	}
}
