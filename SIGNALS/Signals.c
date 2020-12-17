#include "Signals.h"

#include <asm-generic/errno-base.h>
#include <bits/signum-generic.h>
#include <linux/prctl.h>
#include <stddef.h>

#include "../UTILS/Utils.h"

int current = -1;
sigset_t sigset = { 0 };
sigset_t oldset = { 0 };

//! Get the exact bit from the symbol (counting from zero)
char get_bit(char a, char no_of_bit) {
	return (a & (1 << no_of_bit)) >> no_of_bit;
}

//CRITICAL SECTION 2 PARENT PART START-----------------------------------------------
//! Parent's SIGUSR1 handler
void sigusr1_p_handler(int signum) {
	current = 0;
}

//! Parent's SIGUSR2 handler
void sigusr2_p_handler(int signum) {
	current = 1;
}
//CRITICAL SECTION 2 PARENT PART END-----------------------------------------------

//CRITICAL SECTION 3 CHILD PART START------------------------------------------------
//! Parent's SIGCHLD handler
void sigchld_p_handler(int signum) {
	exit(-1);
}
//CRITICAL SECTION 3 CHILD PART END------------------------------------------------

//! Child's SIGUSR1 handler
void sigusr1_c_handler(int signum) {
}

//! Child's SIGUSR2 handler
void sigusr2_c_handler(int signum) {
}

//! Child's SIGHUP (parent's death) handler
void sighup_c_handler(int signum) {
	exit(-1);
}

void setSignalAction(int signal, void (*handler)(int)) {
	struct sigaction sigact = { 0 };
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
	sigact.sa_handler = handler;
	if ((sigaction(signal, &sigact, NULL)) == -1) {
		ERROR("sigaction");
	}
}

void sig_parent(int cpid) {
	// Parent process
	int symbol = 0;

	while (1) {
		symbol = 0;
		for (int i = 0; i < sizeof(char) * 8; ++i) {
			// Wait for SIGUSR1/SIGUSR2/SIGCHLD
//CRITICAL SECTION 4 START
			sigsuspend(&oldset);
//CRITICAL SECTION 4 END
//CRITICAL SECTION 3 PARENT PART END----------------------------------------------------------------
			symbol = (symbol << 1) | current;

			if (i == sizeof(char) * 8 - 1) {
				// If it is the last bit - send signal after writting
				break;
			}

			if (kill(cpid, SIGUSR1) == -1) {
				ERROR("killed");
			}
		}

		if (write(STDOUT_FILENO, &symbol, sizeof(char)) == -1) {
			ERROR("while wrting to console");
		}
//CRITICAL SECTION 3 PARENT PART START----------------------------------------------------------
		if (kill(cpid, SIGUSR1) == -1) {
			if (errno != ESRCH) {
				ERROR("killed");
			}

			exit(-1);
		}


	}

}

void sig_child(char *fileName, int ppid) {
	size_t readbytes = 0;
	char symbol = 0;

	// Prepare for parent's death
	prctl(PR_SET_PDEATHSIG, SIGHUP);

	if (ppid != getppid()) {
		ERROR("parent is dead");
	}

//CRITICAL SECTION 1 END -------------------------------------------------

	// Set up child's signal handler's
	setSignalAction(SIGUSR1, sigusr1_c_handler);
	setSignalAction(SIGUSR2, sigusr2_c_handler);
	setSignalAction(SIGHUP, sighup_c_handler);

	int file = 0;
	if ((file = open(fileName, O_RDONLY)) == -1) {
		ERROR("while opening file");
	}

	// Read from file by symbols
	while ((readbytes = read(file, &symbol, sizeof(char))) == sizeof(char)) {
		// Transfer this symbol by bits
		for (int i = sizeof(char) * 8 - 1; i >= 0; --i) {
			if (!get_bit(symbol, i)) {
//CRITICAL SECTION 2 CHILD PART START --------------------------------------------------
				// bit is zero
				// Send SIGUSR1
				if (kill(ppid, SIGUSR1) == -1) {
					ERROR("killed");
				}
			} else {
				// bit is one
				// Send SIGUSR2
				if (kill(ppid, SIGUSR2) == -1) {
					ERROR("killed");
				}
			}

			// Wait for SIGUSR1 from parent
			sigsuspend(&oldset);
//CRITICAL SECTION 2 CHILD PART END --------------------------------------------------
		}
	}

	if (readbytes == -1) {
		ERROR("while reading");
	}

	close(file);
}

int Task4(int argc, char *argv[]) {

	if (argc != 2) {
		printf("Wrong input arguments!\n");
		exit(-1);
	}

	// Set up parent's signal handlers
	setSignalAction(SIGUSR1, sigusr1_p_handler);
	setSignalAction(SIGUSR2, sigusr2_p_handler);
	setSignalAction(SIGCHLD, sigchld_p_handler);

	// Set SIGUSR1/SIGUSR2 blocking mask (for borh parent and child)
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGUSR1);
	sigaddset(&sigset, SIGUSR2);
	sigaddset(&sigset, SIGCHLD);
	sigaddset(&sigset, SIGHUP);
	sigprocmask(SIG_BLOCK, &sigset, &oldset);

//CRITICAL SECTION 1 START -------------------------------------------------
	
	int ppid = getpid();
	int cpid = 0;
	if ((cpid = fork()) == -1) {
		perror("fork");
		exit(-1);
	}

	if (cpid == 0) {
		// Child process
		sig_child(argv[1], ppid);
	} else {
		// Parent process
		sig_parent(cpid);
	}

	return 0;

}
