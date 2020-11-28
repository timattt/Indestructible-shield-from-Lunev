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
Нет, не всегда, например возможен вариант, что мы сделаем mount таким образом что появится директория с таким же именем (таким же путем до файла), но файлы будут разные. Или возможно такое, что между open файлы переименуются.   
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

Это происходит потому что для определения прав доступа используется константа 666, а не 0666. Ноль в начале обеспечивает, что константа является восьмеричной. Поэтому мы просто имеем неправильную константу. И параметры доступа соответственно тоже неправильные. Но если запустить программу как суперадминистратор через sudo, то она будет игнорировать все права. И все будет работать.

Следовательно изначальная программа не выведет ничего из-за ошибки.
6) фифо в каких случаях open возращает успех при открытии с O_NONBLOCK?
ответ: opening for read-only succeeds even if no one has opened on the write side yet and opening for write-only fails with ENXIO (no such device or address) unless the other end has already been opened.
почему так?
ответ: Одновременно не открывать -> никогда не откроешь
Одновременно открывать -> у тебя нет возможности узнать что ты умрешь если будешь писать в пайп (сигпайп)

## II
1) Есть ли в коде критические секции?   
Есть - конфликт за stdout между детьми. (код: printf и fflush)

## III
1) Есть ли в коде критические секции?   
Есть. Смотреть в коде.   

## IV
1) Если поменять местами форк и сигпроцмаск что будет?
Будет проблема из-за того, что возможна ситуация, когда ребенок отправит сигнал и начнет ждать сигнал от родителя, а потом родитель заблокирует сигналы и начнет ждать сигнала от ребенка - DEAD LOCK.
1) Есть ли в коде критические секции?   
2) Когда процесс уходит из таблицы процессов?      
когда родитель сделал wait     
3) Можно ли сделать так чтобы дети не оставались в таблице?       
Да, заменить дефолтный обработчик SIGCHLD у родителя на SIGIGN    
4) Какие сигналы могут быть разосланы при выходе процесса?   
SIGCHLD   
prctl() - любые (можно тустановить)   
SIGHUP    смерть лидера сессии
SIGPIPE   - если мы читали из пайпа и умерли тогда процессу который в пайп писал будет этот сигнал
SIGIO   - в случае select.
SIGCONT

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

