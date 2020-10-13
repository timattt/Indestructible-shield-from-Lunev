#ifndef FILE_READER
#define FILE_READER

#include "../Base/Base.h"

// DATA
// input file
int INPUT_FILE_FD;
char * INPUT_FILE_NAME;

// STEPS | PROTOTYPES
void rMAKE_STEPS();
void rOPEN_INPUT_FILE();
void rMKFIFO_TRANSFER();
void rOPEN_TRANSFER_FIFO();
void rREAD_WRITER_KEY();
void rGENERATE_WRITER_FIFO_NAME();
void rOPEN_WRITER_FIFO();
void rFCNTL_WRITER_FIFO();
void rREAD_FROM_INPUT_FILE_WRITE_TO_FIFO();
void rCLOSE_ALL();

void RUN_FILE_READER(char * name);

#endif
