#include "ConsoleWriter.h"

void wMAKE_STEPS() {
	wMKFIFO_TRANSFER();
	wOPEN_TRANSFER();
	wGENERATE_WRITER_FIFO_NAME();
	wMKFIFO_WRITER();
	wOPEN_WRITER_FIFO();
	wWRITE_WRITER_KEY_TO_TRANSFER();
	wMAKE_CONNECTION();
	wFCNTL_WRITER_FIFO();
	wREAD_FROM_WRITER_FIFO();
	CLOSE_ALL();
}

void wMKFIFO_TRANSFER() {
	errno = 0;
	if (mkfifo(TRANSFER_FIFO_NAME, 00600) == -1 && errno != EEXIST) {
		ERROR("while making transfer fifo");
	}
}

void wOPEN_TRANSFER() {
	if ((TRANSFER_FIFO_FD = open(TRANSFER_FIFO_NAME, O_WRONLY)) == -1) {
		ERROR("while opening transfer fifo");
	}
}

void wGENERATE_WRITER_FIFO_NAME() {
	WRITER_KEY = getpid();
	MAKE_NAME(WRITER_KEY, WRITER_FIFO_NAME);
}

void wMKFIFO_WRITER() {
	if (mkfifo(WRITER_FIFO_NAME, 00600) == -1) {
		ERROR("while creating writer fifo");
	}
}

void wOPEN_WRITER_FIFO() {
	if ((WRITER_FIFO_FD = open(WRITER_FIFO_NAME, O_RDONLY | O_NONBLOCK)) == -1) {
		ERROR("while opening writer fifo");
	}
}

void wWRITE_WRITER_KEY_TO_TRANSFER() {
	if (write(TRANSFER_FIFO_FD, &WRITER_KEY, sizeof(int)) == -1) {
		ERROR("while writing transfer fifo");
	}
}

void wMAKE_CONNECTION() {
	int count = 0;
	char buf[BUFFER_SIZE];
	for (int i = 0; i < TOTAL_TRY; i++) {
		errno = 0;
		count = read(WRITER_FIFO_FD, buf, BUFFER_SIZE);
		if (count == -1 || errno != 0) {
			ERROR("while establishing connection with reader");
		}
		if (count > 0) {
			write(STDOUT_FILENO, buf, count);
			break;
		}

		sleep(TRY_DELAY);
	}

	if (count == 0) {
		ERROR("waiting time is out");
	}
}

void wFCNTL_WRITER_FIFO() {
	if (fcntl(WRITER_FIFO_FD, F_SETFL, O_RDONLY) == -1) {
		ERROR("while removing O_NONBLOCK parameter from writer fifo");
	}
}

void wREAD_FROM_WRITER_FIFO() {
	int count = 0;
	char buf[BUFFER_SIZE];
	while (1) {
		count = read(WRITER_FIFO_FD, buf, BUFFER_SIZE);
		if (count == -1) {
			ERROR("while reading remaining part of the data");
		}
		if (count == 0) {
			break;
		}
		write(STDOUT_FILENO, buf, count);
	}
}

void CLOSE_ALL() {
	close(TRANSFER_FIFO_FD);
	close(WRITER_FIFO_FD);
}

void RUN_CONSOLE_WRITER() {
	wMAKE_STEPS();
}
