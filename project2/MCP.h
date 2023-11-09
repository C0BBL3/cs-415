#ifndef MCP_H
#define MCP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>

#define _GUN_SOURCE

// Debugging/Clarity
#define true 1
#define false 0

#define MAX_COMMAND_LENGTH 256
#define MAX_PROCESSES 256
#define QUANTUM 1

void signaler(pid_t* pid_ary, int size, int signal);
int modulo(int a, int b);
int get_next_process();
int schedule_next_process();
void print_status(const pid_t pid);

#endif
