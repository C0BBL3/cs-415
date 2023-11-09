#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

void signaler(pid_t* pid_ary, int size, int signal);

int main(int argc, char* argv[])
{
    int n = 3; // Number of child processes
    pid_t pid;
    pid_t pid_ary[n];

   for (int i = 0; i < n; i++) {
        pid = fork();

        if (pid > 0) {
            // Parent process
            pid_ary[i] = pid;
        }

        if (pid == 0) {
            // Child process
            printf("Child Process: %d - Waiting for SIGUSR1...\n", getpid());
            sigset_t sigset;
            int sig;
            // Create an empty sigset_t
            sigemptyset(&sigset);
            // Use sigaddset() to add the SIGUSR1 signal to the set
            sigaddset(&sigset, SIGUSR1);
            // Use sigprocmask() to add the signal set in the sigset for blocking
            sigprocmask(SIG_BLOCK, &sigset, NULL);
            // Wait for the signal
            sigwait(&sigset, &sig);
            printf("Child Process: %d - Received signal: SIGUSR1 - Calling exec().\n", getpid());

            // Call execvp with ./iobound like in lab 4
            // char* args[] = {"./iobound", NULL};
            // execvp(args[0], args);
			execlp("./iobound", "-second", "5", NULL); // child process
			perror("execlp");
            exit(1);
        }

        if (pid < 0) {
            // Fork failed error
            perror("Fork failed");
        }
    }

    // Send signals to child processes
    signaler(pid_ary, n, SIGUSR1); // Send SIGUSR1
    sleep(2); // Sleep for 2 seconds
    signaler(pid_ary, n, SIGSTOP); // Send SIGSTOP
    sleep(2); // Sleep for 2 seconds
    signaler(pid_ary, n, SIGCONT); // Send SIGCONT
    sleep(2); // Sleep for 2 seconds
    signaler(pid_ary, n, SIGINT); // Send SIGINT

    // Wait for child processes to complete
    for (int i = 0; i < n; i++) {
        wait(NULL);
    }

    return 0;
}

void signaler(pid_t* pid_ary, int size, int signal)
{
    // Sleep for three seconds
    sleep(3);
    for (int i = 0; i < size; i++) {
        // Print: Parent process: <parent_pid> - Sending signal: <signal> to child process: <child_pid>
        printf("Parent process: %d - Sending signal: %d to child process: %d\n", getpid(), signal, pid_ary[i]);
        // Send the signal
        kill(pid_ary[i], signal);
    }
}
