#include "Proxy.h"

#include <asm-generic/errno-base.h>
#include <bits/types/struct_timeval.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>

#include "../UTILS/Utils.h"

#define RD 0
#define WR 1
#define CH_BS 131072
#define PT_BS 1024

typedef struct rustem {
	int from[2];
	int to[2];
	char *buffer;
	char *next;
	int buf_size;
	int num_bytes;
} child_;

child_ *proxy = NULL;

int Task5(int argc, char *argv[]) {
	if (argc != 3) {
		ERROR("too few arguments");
	}

	int val = STRING_TO_INT(argv[1]);

	val += 2;

	int fd = open(argv[2], O_RDONLY);
	if (fd == -1) {
		ERROR("while opening file");
	}

	create(val, fd);

	int ret = -1;

	for (int i = 1; i < val - 1; i++) {
		ret = fork();
		if (ret == -1) {
			ERROR("fork");
		}

		if (!ret) {
			child(val, i);
		}
	}

	parent(val);

	return 0;
}

int parse_n(char *str) {
	char *endptr = 0;
	int val = 0;
	errno = 0;

	val = strtoll(str, &endptr, 10);

	if (*endptr != 0 || endptr == str) {
		ERROR("while parsing n");
	}
	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
			|| (errno != 0 && val == 0)) {
		ERROR("while parsing n");
	}

	return (int) val;
}

void create(int val, int fd) {
	proxy = calloc(val, sizeof(*proxy));

	for (int i = 1; i < val - 1; i++) {
		if (pipe(proxy[i].from) == -1) {
			ERROR("pipe1");
		}
		if (pipe(proxy[i].to) == -1) {
			ERROR("pipe2");
		}
		if (fcntl(proxy[i].from[RD], F_SETFL, O_RDONLY | O_NONBLOCK) == -1) {
			ERROR("fcntl1");
		}
		if (fcntl(proxy[i].to[WR], F_SETFL, O_WRONLY | O_NONBLOCK) == -1) {
			ERROR("fcntl2");
		}
	}

	proxy[0].from[RD] = fd;
	proxy[0].from[WR] = -1;
	proxy[0].to[RD] = -1;
	proxy[0].to[WR] = -1;

	proxy[val - 1].from[RD] = -1;
	proxy[val - 1].from[WR] = -1;
	proxy[val - 1].to[RD] = -1;
	proxy[val - 1].to[WR] = STDOUT_FILENO;
}

void child(int n, int current) {
	for (int i = 1; i < n - 1; i++) {
		close(proxy[i].from[RD]);
		close(proxy[i].to[WR]);

		if (i != current) {
			close(proxy[i].from[WR]);
			close(proxy[i].to[RD]);
		}
	}

	char buffer[CH_BS] = { 0 };
	int ret = 0;

	do {
		if ((ret = read(proxy[current].to[RD], buffer, CH_BS)) == -1) {
			ERROR("while reading");
		}
		if (write(proxy[current].from[WR], buffer, ret) == -1) {
			ERROR("while writing");
		}
	} while (ret);

	close(proxy[current].to[RD]);
	close(proxy[current].from[WR]);

	free(proxy);
	exit(EXIT_SUCCESS);
}

void parent(int n) {
	int pow_3 = 3;

	for (int i = n - 2; i > 0; i--) {
		close(proxy[i].from[WR]);
		close(proxy[i].to[RD]);

		if (PT_BS * pow_3 < CH_BS)
			proxy[i].buf_size = PT_BS * pow_3;
		else
			proxy[i].buf_size = CH_BS;
		pow_3 *= 3;
		proxy[i].buffer = calloc(proxy[i].buf_size, sizeof(*proxy[i].buffer));
		proxy[i].next = proxy[i].buffer;
	}

	proxy[n - 1].buf_size = PT_BS;
	proxy[n - 1].buffer = calloc(proxy[n - 1].buf_size,
			sizeof(*proxy[n - 1].buffer));
	proxy[n - 1].next = proxy[n - 1].buffer;

	int nfds = 0;
	while (1) {
		fd_set readfds, writefds;
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);

		int counter = 0;
		for (int i = 0; i < n; i++) {
			if (proxy[i].from[RD] > nfds) {
				nfds = proxy[i].from[RD];
			}
			if (proxy[i].to[WR] > nfds) {
				nfds = proxy[i].to[WR];
			}
			if (proxy[i].from[RD] != -1 && proxy[i + 1].num_bytes == 0) {
				FD_SET(proxy[i].from[RD], &readfds);
				counter++;
			}
			if (proxy[i].to[WR] != -1 && proxy[i].num_bytes != 0) {
				FD_SET(proxy[i].to[WR], &writefds);
				counter++;
			}
		}

		if (!counter) {
			break;
		}

		if (select(nfds + 1, &readfds, &writefds, NULL, NULL) == -1) {
			ERROR("select");
		}

		for (int i = 0; i < n; i++) {
			if (FD_ISSET(proxy[i].from[RD], &readfds)) {
				reading(i);
			}
			if (FD_ISSET(proxy[i].to[WR], &writefds)) {
				writing(i);
			}
		}
	}

	for (int i = 1; i < n; i++) {
		free(proxy[i].buffer);
	}

	free(proxy);
	exit(EXIT_SUCCESS);
}

void reading(int current) {
	int ret = 0;
	if ((ret = read(proxy[current].from[RD], proxy[current + 1].buffer,
			proxy[current + 1].buf_size)) == -1) {
		ERROR("read");
	}
	proxy[current + 1].num_bytes += ret;

	if (!ret) {
		if (close(proxy[current].from[RD]) == -1) {
			ERROR("close");
		}
		if (proxy[current + 1].to[WR] != 1)
			if (close(proxy[current + 1].to[WR]) == -1) {
				ERROR("close");
			}
		proxy[current].from[RD] = -1;
		proxy[current + 1].to[WR] = -1;
	}
}

void writing(int current) {
	int ret = -1;

	if ((ret = write(proxy[current].to[WR], proxy[current].next,
			proxy[current].num_bytes)) == -1) {
		ERROR("write");
	}
	proxy[current].num_bytes -= ret;
	proxy[current].next += ret;

	if (!proxy[current].num_bytes)
		proxy[current].next = proxy[current].buffer;
}
