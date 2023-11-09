#include<stdio.h>
#include <sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

void script_print (pid_t* pid_ary, int size);

int main(int argc,char*argv[])
{
	if (argc != 2)
	{
		printf ("Wrong number of arguments\n");
		exit (0);
	}

	int n = atoi(argv[1]);

	pid_t child_process_pool[n]; 

	char script_args[256];
    snprintf(script_args, sizeof(script_args), "./iobound -seconds %s", argv[1]);

	for (int i = 0; i < n; i++) 
	{
		pid_t child = fork();

		if (child < 0)
		{ // error
			perror("fork");
			exit(EXIT_FAILURE);
		} else if (child == 0)
		{
			execlp("./iobound", "-second", "5", NULL); // child process
			perror("execlp");
            exit(1);
		} 
		else 
		{
			child_process_pool[i] = child;
		}
	}

	for (int i = 0; i < n; i++) {
		waitpid(child_process_pool[i], NULL, 0);
	}

	/*
	*	TODO
	*	#1	declare child process pool
	*	#2 	spawn n new processes
	*		first create the argument needed for the processes
	*		for example "./iobound -seconds 10"
	*	#3	call script_print
	*	#4	wait for children processes to finish
	*	#5	free any dynamic memory
	*/

	return 0;
}


void script_print (pid_t* pid_ary, int size)
{
	FILE* fout;
	fout = fopen ("top_script.sh", "w");
	fprintf(fout, "#!/bin/bash\ntop");
	for (int i = 0; i < size; i++)
	{
		fprintf(fout, " -p %d", (int)(pid_ary[i]));
	}
	fprintf(fout, "\n");
	fclose (fout);

	char* top_arg[] = {"gnome-terminal", "--", "bash", "top_script.sh", NULL};
	pid_t top_pid;

	top_pid = fork();
	{
		if (top_pid == 0)
		{
			if(execvp(top_arg[0], top_arg) == -1)
			{
				perror ("top command: ");
			}
			exit(0);
		}
	}
}


// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <sys/wait.h>

// int main(int argc, char *argv[]) {
//     if (argc != 2) {
//         printf("Wrong number of arguments\n");
//         exit(0);
//     }

//     // Number of child processes to create
//     int n = 5; // You can set this to your desired number

//     // Declare a pool to store child process PIDs
//     pid_t child_pids[n];

//     // Prepare the argument needed for the processes
//     char script_args[256];
//     snprintf(script_args, sizeof(script_args), "./iobound -seconds %s", argv[1]);

//     // Spawn n new processes
//     for (int i = 0; i < n; i++) {
//         pid_t child_pid = fork();

//         if (child_pid == -1) {
//             perror("fork");
//             exit(1);
//         }

//         if (child_pid == 0) {
//             // This is the child process
//             // Execute the script with arguments
//             execlp("/bin/sh", "sh", "-c", script_args, (char *)0);
//             perror("execlp");
//             exit(1);
//         } else {
//             // This is the parent process
//             child_pids[i] = child_pid;
//         }
//     }

//     // Wait for children processes to finish
//     for (int i = 0; i < n; i++) {
//         waitpid(child_pids[i], NULL, 0);
//     }

//     // Free any dynamic memory (not needed in this example)

//     return 0;
// }
