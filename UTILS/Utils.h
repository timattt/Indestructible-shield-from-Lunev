#ifndef UTILS
#define UTILS

// INCLUDES
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

// UTILITIES
void ERROR(const char * mes);
void ERROR_CONCAT(const char * mes, const char * mes1);
void DEBUG(const char * mes);
void MAKE_NAME(int key, char * dest);
long STRING_TO_INT(char * str);

#endif
