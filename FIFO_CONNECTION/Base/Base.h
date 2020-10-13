#ifndef BASE_RW
#define BASE_RW

// CONSTANTS
#define TRANSFER_FIFO_NAME "TRANSFER"
#define NAME_LENGTH 10
#define ALPHABET_SIZE 26
#define BUFFER_SIZE 64

// COMMON DATA
// writer fifo
char WRITER_FIFO_NAME[NAME_LENGTH];
int WRITER_KEY;
int WRITER_FIFO_FD;

// transfer fifo
int TRANSFER_FIFO_FD;

// INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// UTILITIES
void ERROR(const char * mes);
void DEBUG(const char * mes);
void MAKE_NAME(int key, char * dest);

#endif
