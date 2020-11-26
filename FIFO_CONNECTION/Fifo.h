#pragma once

#include <asm-generic/errno-base.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../UTILS/Utils.h"

void MAKE_NAME(int key, char * dest);
void fifo_fileReader(char * INPUT_FILE_NAME);
void fifo_consoleWriter();
int Task1(int argc, char * argv[]);
