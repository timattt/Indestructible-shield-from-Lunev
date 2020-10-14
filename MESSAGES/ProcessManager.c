#include "ProcessManager.h"

void make_steps() {
	make_queue();
	make_processes();
}

void make_queue() {
	if ((QUEUE_ID = msgget(IPC_PRIVATE, IPC_CREAT | 0666)) == -1) {
		ERROR("while creating queue");
	}
}

void make_processes() {
	for (int i = 1; i <= n; i++) {
		int pid = fork();
		// IF ERROR
		if (pid == -1) {
			ERROR("while creating child process");
		}
		// IF CHILD
		if (pid == 0) {
			CHILD_ID = i;
			child_steps();
			return;
		}
		// IF PARENT
		else {
			CHILD_ID = -1;
			continue;
		}
	}

	parent_steps();
}

void child_steps() {
	int trash = 12345;
	receive_msg(QUEUE_ID, &trash, CHILD_ID);
	printf("%d\n", CHILD_ID);
	fflush(0);
	send_msg(QUEUE_ID, trash, n + 1);
}

void parent_steps() {
	for (int i = 1; i <= n; i++) {
		int trash = 12345;
		send_msg(QUEUE_ID, trash, i);
		receive_msg(QUEUE_ID, &trash, n + 1);
	}

	close_queue();
}

void close_queue() {
	if (msgctl(QUEUE_ID, IPC_RMID, NULL) == -1) {
		ERROR("while closing queue");
	}
}

void send_msg(int queue, int data, int type) {
	struct msgbuf buf = {type, data};
	if (msgsnd(queue, &buf, sizeof(int), MSG_NOERROR) == -1) {
		ERROR("while sending message");
	}
}

void receive_msg(int queue, int * dest, int type) {
	struct msgbuf buf = {123, 123};
	if (msgrcv(queue, &buf, sizeof(int), type, MSG_NOERROR) == -1) {
		ERROR("while receiving message");
	}
}
