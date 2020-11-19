#ifndef PROC_MANAGER
#define PROC_MANAGER

// INCLUDES
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
#include "../UTILS/Utils.h"

// DATA
int QUEUE_ID;
int CHILD_ID;
int n;

// STEPS
void make_steps();
void make_queue();
void make_processes();
void child_steps();
void parent_steps();
void close_queue();

// OTHER
void send_msg(int queue, int data, int type);
void receive_msg(int queue, int * dest, int type);

struct msgbuf {
	long type;
	int data;
};

#endif
