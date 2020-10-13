# Indestructible-shield-from-Lunev
This repository contains programs that will serve as a shield against my expulsion from the flagship of Russian science

# Первое задание (fifo)
## Сжатый алгоритм системных вызовов:
### Reader from file:
open file (O_RDONLY | O_NONBLOCK)   
mkfifo transfer 00600   
open transfer O_RDONLY   
read writer key from transfer    
open writer fifo by key (O_WRONLY | O_NONBLOCK)    
fcntl writer key (O_WRONLY)    
loop: read from file, write to writer fifo    

### Writer to console:
mkfifo transfer    
open transfer (O_WRONLY)    
mkfifo using writer pid (as key)    
open writer fifo (O_RDONLY | O_NONBLOCK)   
write key to transfer    
N times loop: read from writer fifo, sleep(DELAY)    
fcntl writer fifo (O_RDONLY)       
read the remaining    

# Немного красной символики
![](https://github.com/timattt/Indestructible-shield-from-Lunev/blob/master/MASCOT_STAR.svg)
