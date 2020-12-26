# Indestructible shield from Lunev
Этот репозиторий содержит детально разобранные решения задач и контрольных Лунева.
Еще тут есть полный конспект всех семинаров.
За это отдельное спасибо [Ване](https://vk.com/zoomzero).

# Навигация



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
Если это имя было последней ссылкой на файл и больше нет процессов, которые держат этот файл открытым, данный файл удаляется и место, которое он занимает освобождается для дальнейшего использования, т.е. будет 1 гиг свободного места.    
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

Это происходит потому что для определения прав доступа используется константа 666, а не 0666. Ноль в начале обеспечивает, что константа является восьмеричной. Поэтому мы просто имеем неправильную константу. И параметры доступа соответственно тоже неправильные. Но если запустить программу как суперадминистратор через sudo, то она будет игнорировать все права. Либо возможен вариант, когда такой fifo уже создан с правильными правами. Тогда все будет работать.

Следовательно изначальная программа не выведет ничего из-за ошибки.   

6) Для fifo в каких случаях open возращает успех при открытии с O_NONBLOCK?    
Открытие только для чтения завершается успешно, даже если на стороне записи еще никто не открывался, а открытие только для записи завершается неудачно с ENXIO, если другой конец еще не был открыт.
Потому что если одновременно не открывать, то никогда не получится открыть.
А если одновременно открывать, то у тебя нет возможности узнать, что ты умрешь если будешь писать в пайп (будет сигпайп).

7)
```
main() {
	stat(&st, filename);
	if (st.n_link == 1) {
		err = unlink(filename);
		if (err == 0) {
			// проверяем свободное место
		}
	}
}
```
если filename, тот же файл, который запустился.


## II
1) Есть ли в коде критические секции?   
Есть - конфликт за stdout между детьми. (код: printf и fflush)

## III
1) Есть ли в коде критические секции?   
Есть. Смотреть в коде.   

## IV
1) Если поменять местами форк и сигпроцмаск что будет?    
Будет проблема из-за того, что возможна ситуация, когда ребенок отправит сигнал и начнет ждать сигнал от родителя, а потом родитель заблокирует сигналы и начнет ждать сигнала от ребенка - DEAD LOCK. (сигнал от ребенка может быть обработан раньше suspend (тк процмаск ещё не установлен), тогда родитель и ребенок повиснут в своих suspend-ах)
2) Есть ли в коде критические секции?   
Есть - смотреть в коде.   
* 177-113   
боимся что родитель умрет прежде чем сделаем prctl   
* child: 130-146   
parent: 19-29   
за маску входных сигналов родителя   
* child: 31-36   
parent: 88-72   
за маску входных сигналов ребенка   
* parent: 69-71   
хэндлеры родителя с самим родителем за переменную current   
3) Когда процесс уходит из таблицы процессов?      
когда он завершился и родитель сделал wait.     
4) Можно ли сделать так чтобы дети не оставались в таблице?       
Да, заменить дефолтный обработчик SIGCHLD у родителя на SIGIGN    
5) Какие сигналы могут быть разосланы при выходе процесса?   
* SIGCHLD - при смерти ребенка сингчайлд родителю  
* prctl() - можно выставить, чтобы любой сигнал отправлялся в результате смерти детям, которые высталили prctl.
* SIGHUP - смерть лидера сессии все в сессии сигнал.    
* SIGPIPE - если мы читали из пайпа и умерли тогда процессу который в пайп писал будет этот сигнал   
* SIGCONT посылается, если в сессии были были процессы с SIGSTOP, а лидер сессии умер, то все процессам разошлется этот сигнал.    
6) Что происходит при segfault. Кто его детектирует и как?    
Исполняется процесс, работает с памятью, процессор мапит память, если в реальности такой страницы нет, то генерируется pagefault, запускается обработчик исключения в ядре. Далее два варианта: если нужная страница будет обнаружена на жеством диске, то она подгружается и процесс вновь запускается с того же места. Если страницы нет, то тогда процессу sigseg. Все ресурсы чистятся. Если была оболочка bash, то ошибку туда.    
Чтобы доказать нужно навесить strace на оболочку и увидеть.    

Детектирует процессор, он посылает исключение pagefault ОС, а ОС своим обработчиком уже его обрабатывает и делает, что нужно.
А возникает он при обращении в недозволенную память, при попытке, например, изменить литерал или при считывании с плохого указателя.
Это можно доказать, если повесить strace на bash.    
7) На какие сигналы нельзя поставить обработчик:
sigstop, sigkill, sigcont    

# Сжатые алгоритмы решений   
## Первое задание (fifo)

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
