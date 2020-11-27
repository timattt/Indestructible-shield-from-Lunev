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

	//---------------
	// Critical section 1
	// Readers conflict with each other for second fifo
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

	int WRITER_KEY = 0;
	if (read(TRANSFER_FIFO_FD, &WRITER_KEY, sizeof(int)) == -1) {
		ERROR("while reading key from transfer fifo");
	}
	char WRITER_FIFO_NAME[NAME_LENGTH] = { 0 };
	MAKE_NAME(WRITER_KEY, WRITER_FIFO_NAME);

	//---------------

	int WRITER_FIFO_FD = 0;
	if ((WRITER_FIFO_FD = open(WRITER_FIFO_NAME, O_WRONLY | O_NONBLOCK)) == -1) {
		ERROR("while opening writer fifo");
	}

	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//---------------

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

	//---------------
	// Critical section 2
	// Conflict between reader and writer for second fifo
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

	if (write(TRANSFER_FIFO_FD, &WRITER_KEY, sizeof(int)) == -1) {
		ERROR("while writing transfer fifo");
	}

	//---------------

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

	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//---------------

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


/*
-----------------------------------------
ВОПРОСЫ:
-----------------------------------------
1. Есть ли в коде критические секции?
Есть. Смотреть в коде.
2. Даны тебе 2 файловых дискриптора, тебе нужно написать программу, которая говорит, это дискрипторы одного и того же файла или нет.
int SAME_FD_SAME_FILES(int fd1, int fd2) {
	struct stat stat1, stat2;
	if (fstat(fd1, &stat1) < 0)
		return -1;
	if (fstat(fd2, &stat2) < 0)
		return -1;
	return (stat1.st_dev == stat2.st_dev) && (stat1.st_ino == stat2.st_ino);
}
3. Пусть теперь предыдущая прога на вход принимает два пути к двум файлам, потом делает open на каждый из них. А дальше все то же самое. Вопрос: если я дам два одинаковых пути, что выдаст твоя прога? Всегда ли?
Нет, не всегда, например возможен вариант, что мы сделаем mount таким образом что появится директория с таким же именем (таким же путем до файла), но файлы будут разные. Или возможно такое, что между open файлы переименуются.
4. дана флешка на гиг, на ней файл на гиг, я делаю успешный unlink, сколько свободного места на флешке?
Если это имя является посленим, то файл удалится после завершения всех процессов в том числе и текущего где он открыт, и будет 1 гиг свободного места.
5. Что выведет прога:

int main() {
    char buf[3] = { 0 };
    mkfifo("fifo", 666);
    int fd = open("fifo", O_RDWR);
    write(fd, "aaa", 3);
    fd = open("fifo", O_RDONLY | O_NONBLOCK);
    write(1, buf, read(fd, buf, 3));
}

Программу можно переписать в виде более удобном для отладки.

	char buf[3] = { 0 };
	if (mkfifo("fifo", 666) < 0) {
		ERROR("mkfifo");
	}

	int fd = 0;
	if ((fd = open("fifo", O_RDWR)) < 0) {
		ERROR("open 1");
	}

	if (write(fd, "aaa", 3) < 0) {
		ERROR("write 1");
	}

	if ((fd = open("fifo", O_RDONLY | O_NONBLOCK)) < 0) {
		ERROR("open 2");
	}

	int count = 0;
	if ((count = read(fd, buf, 3)) < 0) {
		ERROR("read");
	}

	if (write(1, buf, count) < 0) {
		ERROR("write 2");
	}
	
После запуска вылетает ошибка

Error with code 13, open 1
Printing message:
Permission denied

Это происходит потому что для определения прав доступа используется константа 666, а не 0666. Ноль в начале обеспечивает, что константа является восьмеричной. Поэтому мы просто имеем неправильную константу. И параметры доступа соответственно тоже неправильные. Вторая возможная причина ошибки - тот факт, что в mkfifo параметры доступа для файла задаются формулой (mode | umask), поэтому если не сделать umask(0) в начале, то umask может быть заполнен мусором. Что тоже приведет к неправильному значению параметра прав доступа.

Следовательно изначальная программа не выведет ничего из-за ошибки.
-----------------------------------------
*/
