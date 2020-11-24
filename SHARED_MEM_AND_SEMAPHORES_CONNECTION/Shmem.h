#pragma once

#include <fcntl.h>
#include <stddef.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>

#include "../UTILS/Utils.h"

int Task3(int argc, char * argv[]);
void consoleWriter(int semid, char * shmem);
void fileReader(char * fileName, int semid, char * shmem);
void common(int * semid, int * shmid, char ** shmem);
