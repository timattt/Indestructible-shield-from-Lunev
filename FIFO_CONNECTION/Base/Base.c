#include "Base.h"

void MAKE_NAME(int key, char * dest) {
	for (int i = 0; i < NAME_LENGTH; i++) {
		dest[i] = 'a' + (key % ALPHABET_SIZE);
		key /= ALPHABET_SIZE;
	}
}
