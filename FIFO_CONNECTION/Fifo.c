#include "Fifo.h"

#define NAME_LENGTH 10
#define ALPHABET_SIZE 26
#define TRANSFER_FIFO_NAME "transfer"
#define BUFFER_SIZE 64
#define TOTAL_TRY 5
#define TRY_DELAY 1

void MAKE_NAME(int key, char * dest) {
	for (int i = 0; i < NAME_LENGTH; i++) {
		dest[i] = 'a' + (key % ALPHABET_SIZE);
		key /= ALPHABET_SIZE;
	}
}

void fifo_fileReader(char * INPUT_FILE_NAME) {
	//---------------

	int INPUT_FILE_FD = 0;
	if ((INPUT_FILE_FD = open(INPUT_FILE_NAME, O_RDONLY)) == -1) {
		ERROR("while opening input file");
	}

	//---------------

	errno = 0;
	if (mkfifo(TRANSFER_FIFO_NAME, 0666) == -1 && errno != EEXIST) {
		ERROR("while creating transfer fifo");
	}

	//---------------

	int TRANSFER_FIFO_FD = 0;
	if ((TRANSFER_FIFO_FD = open(TRANSFER_FIFO_NAME, O_RDONLY)) == -1) {
		ERROR("while opening transfer fifo");
	}

// Critical section 1
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// File readers conflicts for pid inside common pipe.
	
	int WRITER_KEY = 0;
	if (read(TRANSFER_FIFO_FD, &WRITER_KEY, sizeof(int)) == -1) {
		ERROR("while reading key from transfer fifo");
	}
	char WRITER_FIFO_NAME[NAME_LENGTH] = { 0 };
	MAKE_NAME(WRITER_KEY, WRITER_FIFO_NAME);

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	
	//---------------

// Critical section 3 reader part
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// reader and writer conflicts for unique fifo file descriptor.
	
	int WRITER_FIFO_FD = 0;
	if ((WRITER_FIFO_FD = open(WRITER_FIFO_NAME, O_WRONLY | O_NONBLOCK)) == -1) {
		ERROR("while opening writer fifo");
	}

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	if (fcntl(WRITER_FIFO_FD, F_SETFL, O_WRONLY) == -1) {
		ERROR("while removing O_NONBLOCK flag from writer fifo");
	}

	//---------------

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

	//---------------

	if (close(TRANSFER_FIFO_FD) == -1) {
		ERROR("while closing transfer fifo");
	}
	if (close(WRITER_FIFO_FD) == -1) {
		ERROR("while closing writer fifo");
	}
	if (close(INPUT_FILE_FD) == -1) {
		ERROR("while closing input file");
	}

	remove(WRITER_FIFO_NAME);
	remove(TRANSFER_FIFO_NAME);

	//---------------
}

void fifo_consoleWriter() {
	//---------------

	errno = 0;
	if (mkfifo(TRANSFER_FIFO_NAME, 00600) == -1 && errno != EEXIST) {
		ERROR("while making transfer fifo");
	}

	//---------------

	int TRANSFER_FIFO_FD = 0;
	if ((TRANSFER_FIFO_FD = open(TRANSFER_FIFO_NAME, O_WRONLY)) == -1) {
		ERROR("while opening transfer fifo");
	}

	//---------------

	int WRITER_KEY = getpid();
	char WRITER_FIFO_NAME[NAME_LENGTH] = { 0 };
	MAKE_NAME(WRITER_KEY, WRITER_FIFO_NAME);

	//---------------

	if (mkfifo(WRITER_FIFO_NAME, 00600) == -1) {
		ERROR("while creating writer fifo");
	}

	//---------------

	int WRITER_FIFO_FD = 0;
	if ((WRITER_FIFO_FD = open(WRITER_FIFO_NAME, O_RDONLY | O_NONBLOCK)) == -1) {
		ERROR("while opening writer fifo");
	}

// Crtical section 2
// Writers with each other for common fifo
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	
	if (write(TRANSFER_FIFO_FD, &WRITER_KEY, sizeof(int)) == -1) {
		ERROR("while writing transfer fifo");
	}

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	
	//---------------

//Critical section 3 writer part
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// reader and writer conflicts for unique fifo file descriptor.
	
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
	
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	if (fcntl(WRITER_FIFO_FD, F_SETFL, O_RDONLY) == -1) {
		ERROR("while removing O_NONBLOCK parameter from writer fifo");
	}

	//---------------

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

	//---------------

	close(TRANSFER_FIFO_FD);
	close(WRITER_FIFO_FD);

	remove(WRITER_FIFO_NAME);
	remove(TRANSFER_FIFO_NAME);

	//---------------
}

int Task1(int argc, char * argv[]) {
	if (argc < 2) {
		ERROR("Too few arguments!");
	}
	if (argv[1][0] == 'r') {
		if (argc < 3) {
			ERROR("Too few arguments!");
		}
		fifo_fileReader(argv[2]);
	}
	if (argv[1][0] == 'w') {
		fifo_consoleWriter();
	}

	return 0;
}
