# ☭ Indestructible shield from Lunev ☭
This repository contains programs that will serve as a shield against my expulsion from the flagship of Russian science   

![](https://www.dualshockers.com/wp-content/uploads/2016/05/Screenshot-2016-05-17-12.07.14.png)

# Дневник красного воина ☭
* Сделал первые два задания, жду сдачи...
* Лунев отменил семинар, + неделя покодить
* Код первых двух задач прошел ☭.
* К обеим прогам получил первый вопрос.
* Ответ на первый вопрос ко второй проге правильный. Ответ на первый вопрос ко второй проге неполный. 

# Вопросы к задачам
## I
1) Есть ли в коде критические секции?   
Есть - системные вызовы чтения и записи во время работы с transfer fifo, конфликт за память и часть кода, где ConsoleWriter ждет фиксированное время, конфликт за память, которая ждет передачи.   
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
Выдаст 1, если файл не является fifo.   
4) дана флешка на гиг, на ней файл на гиг, я делаю успешный unlink, сколько свободного места на флешке?    
unlink удаляет имя из файловой системы. Если файл больше нигде не открыт (т.е. это имя последнее), то 1 гиг, иначе 0.
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

Ничего.


## II
1) Есть ли в коде критические секции?   
Есть - конфликт за stdout между детьми. (код: printf и fflush)

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
* start
* mutex
* full
* alive_r
* alive_w
* die

### Reader from file:
* open file (O_RDONLY)
* shmget (IPC_CREAT | 0600)
* shmat
* semget (IPC_CREAT | 0600)
* semop 1: (ошибка EAGAIN не критична)
* * [start, 0, IPC_NOWAIT]
* * [start, 1, 0]
* semop 2:
* * [die, -2, IPC_NOWAIT]
* * [die, 2, 0]
* * [alive_w, -1, SEM_UNDO]
* * [full, 1, SEM_UNDO]
* * [full, -1, 0]
* semop 3:
* * [alive_r, 0, 0]
* * [die, -1, SEM_UNDO]
* semop 4:
* * [die, 0, 0]
* loop:
* * read from file into buf
* * semop 5:
* * * [die, 0, IPC_NOWAIT]
* * semop 6:
* * * [die, 0, IPC_NOWAIT]
* * * [full, 0, 0]
* * * [mutex, -1, SEM_UNDO]
* * copy from buf to shared mem
* * semop 7:
* * * [full, 1, 0]
* * * [mutex, 1, SEM_UNDO]
* close file
* semop 8:
* * [full, 0, 0]
* * [die, 0, IPC_NOWAIT]
* shmdt

### Writer to console:
* shmget (IPC_CREAT | 0600)
* shmat
* semget (IPC_CREAT | 0600)
* semop 1:
* * [start, 0, IPC_NOWAIT]
* * [start, 1, 0]
* semop 2:
* * [die, -2, IPC_NOWAIT]
* * [die, 2, 0]
* * [alive_r, -1, SEM_UNDO]
* semop 3:
* * [alive_w, 0, 0]
* * [die, -1, SEM_UNDO]
* semop 4:
* * [die, 0, 0]
* loop:
* * semop 5: (ошибка EAGAIN выводит из цикла)
* * * [die, 0, IPC_NOWAIT]
* * semop 6: (ошибка EAGAIN выводит из цикла)
* * * [die, 0, IPC_NOWAIT]
* * * [full, -1, 0]
* * * [mutex, -1, SEM_UNDO]
* * copy to buf from shared mem
* * semop 7:
* * * [mutex, 1, SEM_UNDO]
* * write buf to console
* shmdt
* shmctl (IPC_RMID)
* semctl (IPC_RMID)











# Немного красной символики
![](https://github.com/timattt/Indestructible-shield-from-Lunev/blob/master/MASCOT_STAR.svg)
