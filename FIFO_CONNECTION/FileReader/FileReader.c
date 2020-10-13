#include "FileReader.h"

void rMAKE_STEPS() {
	rOPEN_INPUT_FILE();
	rMKFIFO_TRANSFER();
	rOPEN_TRANSFER_FIFO();
	rREAD_WRITER_KEY();
	rGENERATE_WRITER_FIFO_NAME();
	rOPEN_WRITER_FIFO();
	rFCNTL_WRITER_FIFO();
	rREAD_FROM_INPUT_FILE_WRITE_TO_FIFO();
	rCLOSE_ALL();
}

void rOPEN_INPUT_FILE() {
	if ((INPUT_FILE_FD = open(INPUT_FILE_NAME, O_RDONLY)) == -1) {
		ERROR("while opening input file");
	}
}

void rMKFIFO_TRANSFER() {
	errno = 0;
	if (mkfifo(TRANSFER_FIFO_NAME, 0666) == -1 && errno != EEXIST) {
		ERROR("while creating transfer fifo");
	}
}

void rOPEN_TRANSFER_FIFO() {
	if ((TRANSFER_FIFO_FD = open(TRANSFER_FIFO_NAME, O_RDONLY)) == -1) {
		ERROR("while opening transfer fifo");
	}
}

void rREAD_WRITER_KEY() {
	if (read(TRANSFER_FIFO_FD, &WRITER_KEY, sizeof(int)) == -1) {
		ERROR("while reading key from transfer fifo");
	}
}

void rGENERATE_WRITER_FIFO_NAME() {
	MAKE_NAME(WRITER_KEY, WRITER_FIFO_NAME);
}

void rOPEN_WRITER_FIFO() {
	if ((WRITER_FIFO_FD = open(WRITER_FIFO_NAME, O_WRONLY | O_NONBLOCK)) == -1) {
		ERROR("while opening writer fifo");
	}
}

void rFCNTL_WRITER_FIFO() {
	if (fcntl(WRITER_FIFO_FD, F_SETFL, O_WRONLY) == -1) {
		ERROR("while removing O_NONBLOCK flag from writer fifo");
	}
}

void rREAD_FROM_INPUT_FILE_WRITE_TO_FIFO() {
	int count = 0;
	char buffer[BUFFER_SIZE];
	while ((count = read(INPUT_FILE_FD, buffer, BUFFER_SIZE)) != 0) {
		// IF ERROR OCURED
		if (count == -1) {
			ERROR("while reading from input file");
		}

		errno = 0;
		count = write(WRITER_FIFO_FD, buffer, count);
		if ((count == 0 && errno == EPIPE) || count < 0) {
			ERROR("while writing to writer fifo");
		}
	}
}

void rCLOSE_ALL() {
	if (close(TRANSFER_FIFO_FD) == -1) {
		ERROR("while closing transfer fifo");
	}
	if (close(WRITER_FIFO_FD) == -1) {
		ERROR("while closing writer fifo");
	}
	if (close(INPUT_FILE_FD) == -1) {
		ERROR("while closing input file");
	}
}

void RUN_FILE_READER(char * name) {
	// INIT
	INPUT_FILE_NAME = name;

	// STEPS
	rMAKE_STEPS();
}
