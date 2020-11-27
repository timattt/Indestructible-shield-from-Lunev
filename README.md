# Indestructible shield from Lunev
This repository contains programs that will serve as a shield against my expulsion from the flagship of Russian science   

# Вопросы к задачам
## I
1) Есть ли в коде критические секции?   
Есть. Смотреть в коде.   
2) Даны тебе 2 файловых дискриптора, тебе нужно написать программу, которая говорит, это дискрипторы одного и того же файла или нет.   
```
int SAME_FD_SAME_FILES(int fd1, int fd2) {
	struct stat stat1, stat2;
	if (fstat(fd1, &stat1) < 0)
		return -1;
	if (fstat(fd2, &stat2) < 0)
		return -1;
	return (stat1.st_dev == stat2.st_dev) && (stat1.st_ino == stat2.st_ino);
}
```   
3) Пусть теперь предыдущая прога на вход принимает два пути к двум файлам, потом делает open на каждый из них. А дальше все то же самое. Вопрос: если я дам два одинаковых пути, что выдаст твоя прога? Всегда ли?   
Нет, не всегда, например возможен вариант, что мы сделаем mount таким образом что появится директория с таким же именем, но файлы будут разные. Или возможно такое, что между open файлы переименуются или как-то изменятся.
4) дана флешка на гиг, на ней файл на гиг, я делаю успешный unlink, сколько свободного места на флешке?    
Если это имя является посленим, то файл удалится после завершения всех процессов в том числе и текущего где он открыт, и будет 1 гиг свободного места.    
5) Что выведет прога:
```
int main() {
    char buf[3] = { 0 };
    mkfifo("fifo", 666);
    int fd = open("fifo", O_RDWR);
    write(fd, "aaa", 3);
    fd = open("fifo", O_RDONLY | O_NONBLOCK);
    write(1, buf, read(fd, buf, 3));
}
```

Программу можно переписать в виде более удобном для отладки.
```
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
```

После запуска вылетает ошибка
```
Error with code 13, open 1
Printing message:
Permission denied
```

Это происходит потому что для определения прав доступа используется константа 666, а не 0666. Исходя из того, что права для fifo в системном вызове mkfifo задаются выражением (mode & ~umask), где mode - параметр. Получается, что может произойти заполнение мусором значения маски. Поэтому и будут проблемы с доступом.

Следовательно программа не выведет ничего.

## II
1) Есть ли в коде критические секции?   
Есть - конфликт за stdout между детьми. (код: printf и fflush)

## III
1) Есть ли в коде критические секции?   
Есть. Смотреть в коде.   

# Контрольные
[Полусеместровая](https://github.com/timattt/Indestructible-shield-from-Lunev/blob/master/HalfSemesterKR/readme.md)      

# Первое задание (fifo)
## Сжатый алгоритм системных вызовов:
### Reader from file:
* open file (O_RDONLY | O_NONBLOCK)   
* mkfifo transfer 00600   
* open transfer O_RDONLY   
* read writer key from transfer    
* open writer fifo by key (O_WRONLY | O_NONBLOCK)    
* fcntl writer key (O_WRONLY)    
* loop: 
* * read from file 
* * write to writer fifo    

### Writer to console:
* mkfifo transfer    
* open transfer (O_WRONLY)    
* mkfifo using writer pid (as key)    
* open writer fifo (O_RDONLY | O_NONBLOCK)   
* write key to transfer    
* loop: 
* * read from writer fifo
* * sleep(DELAY)    
* fcntl writer fifo (O_RDONLY)       
* read the remaining    

# Второе задание (messages)
## Сжатый алгоритм:
* Создаем очередь.
* Пораждаем n дочерних процессов.
* Родитель сначала отправляет сообщение типа (n) с любым содержимым, потом ждет сообщение типа (n + 1).
* Каждый сын сначала ждет сообщение типа своего номера, а потом отправляет сообщение   типа (n + 1).
* Закрываем очередь.

# Третье задание (shared mem)
## Сжатый алгоритм системных вызовов:
### Семафоры:
* CW_CUR
* CW_PRE
* FR_CUR
* FR_PRE
* MUTEX
* FULL
* CTL
* SEM_NUM

### Reader from file:
* open file (O_RDONLY)
* shmget (IPC_CREAT | 0666)
* shmat
* semget (IPC_CREAT | 0666)
* semop 1:
* * [FR_CUR, 0, IPC_NOWAIT]
* * [FR_PRE, 0, IPC_NOWAIT]
* * [FR_CUR, +1, SEM_UNDO]
* semop 2:
* * [CW_CUR, -1, 0)]
* * [CW_CUR, +1, 0]
* * [CW_PRE, +1, SEM_UNDO]
* semop 3:
* * [FULL   , +1, SEM_UNDO]
* * [FULL   , -1, 0]
* loop:
* * semop 4:
* * * [CW_CUR, -1, IPC_NOWAIT]
* * * [CW_CUR, +1, 0]
* * * [FULL, 0, 0]
* * * [MUTEX, 0, 0]
* * * [MUTEX, +1, SEM_UNDO]
* * copy from file to shared mem
* * semop 5:
* * * [MUTEX , -1, SEM_UNDO]
* * * [FULL  , +1, 0]
* close file
* shmdt

### Writer to console:
* shmget (IPC_CREAT | 0666)
* shmat
* semget (IPC_CREAT | 0666)
* semop 1:
* * [CW_CUR,  0, IPC_NOWAIT]
* * [CW_PRE,  0, IPC_NOWAIT]
* * [CW_CUR, +1, SEM_UNDO]
* semop 2:
* * [FR_CUR, -1, 0]
* * [FR_CUR, +1, 0]
* * [FR_PRE, +1, SEM_UNDO]
* loop:
* * semop 3:
* * * [FR_CUR, -1, IPC_NOWAIT]
* * * [FR_CUR, +1, 0]
* * * [MUTEX, 0, 0]
* * * [MUTEX, +1, SEM_UNDO]
* * * [FULL, -1, 0]
* * copy to stdout from shared mem
* * semop 4:
* * * [MUTEX, -1, SEM_UNDO]
* shmdt
* shmctl (IPC_RMID)
* semctl (IPC_RMID)

