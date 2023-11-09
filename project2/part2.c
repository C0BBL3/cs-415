#include "MCP.h"
#include "string_parser.h"

pid_t pid_array[MAX_PROCESSES]; // Array to store child process IDs
int process_count = 0; // Number of child processes

void signaler(pid_t* pid_array, int process_count, int signal)
{
    // Sleep for two seconds
    sleep(2);
    for (int i = 0; i < process_count; i++) {
        // Print: Parent process: <parent_pid> - Sending signal: <signal> to child process: <child_pid>
        printf("Parent process: %d - Sending signal: %s to child process: %d\n", getpid(), strsignal(signal), pid_array[i]);
        fflush(stdout);
        // Send the signal
        kill(pid_array[i], signal);
    }
}

int main(int argc, char *argv[]) 
{
    // Check command line arguments and open the input file
    // If no input, exit
    if (argc != 2) 
    {
        fprintf(stderr, "Usage: %s input_file\n", argv[0]);
        fflush(stdout);
        exit(1);
    }

    FILE *input_file = fopen(argv[1], "r");
    if (input_file == NULL) 
    {
        perror("Error opening input file");
        exit(1);
    }

    // Create an empty sigset_t
    int signal;
    sigset_t sigset;
    sigemptyset(&sigset);
    // Use sigaddset() to add the SIGUSR1 signal to the set
    sigaddset(&sigset, SIGUSR1);
    // Use sigprocmask() to add the signal set in the sigset for blocking
    sigprocmask(SIG_BLOCK, &sigset, NULL);

    // Read and fork child processes from the input file
	command_line token_buffer;

	// Declare line_buffer
	size_t len = MAX_COMMAND_LENGTH;
	char* line_buf = malloc(MAX_COMMAND_LENGTH);

    while (getline(&line_buf, &len, input_file) != EOF && process_count < MAX_PROCESSES) 
    {
        token_buffer = str_filler(line_buf, " ");

        // Fork a child process
        pid_t pid = fork();

        if (pid > 0) 
        {
            // Parent process
            pid_array[process_count] = pid;
            process_count++;
        } 
        else if (pid == 0) 
        {
            // Child process
            printf("Child Process: %d - Waiting for SIGUSR1...\n", getpid());
            fflush(stdout);
            
            // Wait for the signal
            sigwait(&sigset, &signal);

            // Signal recieved starting process
            printf("Child Process: %d - Received signal: %s - Calling exec().\n", getpid(), strsignal(signal));
            fflush(stdout);

            // Execute the command using exec
            if (execvp(token_buffer.command_list[0], token_buffer.command_list) == -1) 
            {
                // Child had issue executing command so exit
                perror("Exec error");
                exit(1);
            }
        } 
        else
        {
            perror("Fork error");
            exit(EXIT_FAILURE);
        }

        // Free tokens and reset variable
        free_command_line(&token_buffer);
        memset(&token_buffer, 0, 0);   
    }

    // Free line buffer
    free(line_buf);

    // Close the input file
    fclose(input_file);

    // Send signals to child processes
    // Sleep's are in 'signaler'
    signaler(pid_array, process_count, SIGUSR1); // Emit SIGUSR1
    signaler(pid_array, process_count, SIGSTOP); // Emit SIGSTOP after 2 seconds
    signaler(pid_array, process_count, SIGCONT); // Emit SIGCONT after 2 seconds

    fflush(stdout);
    printf("Waiting for child processes to finish\n");

    // Wait for child processes to finish
    for (int i = 0; i < process_count; i++) 
    {
        int status;
        waitpid(pid_array[i], &status, 0);
    }

    fflush(stdout);
    printf("Finished!\n");

    return 0;
}