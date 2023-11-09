#include "MCP.h"
#include "string_parser.h"

pid_t pid_array[MAX_PROCESSES]; // Array to store child process IDs
int pid_array_working[MAX_PROCESSES]; // Array to store if each PID index is currently working
int process_count = 0; // Number of child processes
int current_process = 0; // Current process index (of pid_array)
int next_process = 0; // Next process index (of pid_array)
int working = true; // Bool for If any children are working


// Need modulo function because it gives floating point exception
// I didn't make the rules it wouldn't run w/o this function
int modulo(int a, int b)
{ 
    if (a < b) 
    {
        return a;
    }
    else if (a == b) 
    {
        return 0;
    }
    else
    {
        while (a > b) 
        {
            a -= b;
        }

        return a;
    }
}

int get_next_process() 
{
    // Get next process index
    int np = modulo(current_process + 1, process_count);

    // Check if next process has exited
    int status = 1;
    
    int w = waitpid(pid_array[np], &status, WNOHANG | WUNTRACED | WCONTINUED | WSTOPPED);
    if (w < 0) {
        perror("waitpid");
        // Dont want to return/exit 
        // Because w < 0 when next process is stopped
    }

    // If next process has exited store that it is not working
    if (WIFEXITED(status) == 1) 
    {
        pid_array_working[np] = 0;
    }

    // If next process has exited or is not working
    while (WIFEXITED(status) == 1 || pid_array_working[np] == 0) 
    {

        // Get next-next process index
        np = modulo(np + 1, process_count);

        // Check if next-next process has exited
        w = waitpid(pid_array[np], &status, WNOHANG | WUNTRACED | WCONTINUED | WSTOPPED);
        if (w < 0) {
            perror("waitpid");
            // Dont want to return/exit 
            // Because w < 0 when next process is stopped
        }

        // If next-next process has exited store that it is not working
        if (WIFEXITED(status) == 1) 
        {
            pid_array_working[np] = 0;
        }

        // If next process loops back to current process 
        // And even current process is finished
        // Return -1 to tell parent its finished
        if (np == current_process && WIFEXITED(status) == 1) 
        {
            return -1;
        }
        // If the current process is not finished
	    // Return the next-next process a.k.a the current process
        else if (np == current_process && WIFEXITED(status) != 1) 
        {
            return np;
        }  
    }

    return np;
}

int schedule_next_process() 
{
    next_process = get_next_process();

    // Finished so stop alarm loop
    if (next_process == -1) 
    {
        return false; 
    }

    // If next process == current process return true 
    // To keep the process running 
    // W/o wasting time stopping and starting
    if (next_process == current_process) 
    { 
        printf("\nAlarm! Continuing: %d\n", pid_array[next_process]);
        return true; 
    }

    printf("\nAlarm! Halting: %d Starting: %d\n", pid_array[current_process], pid_array[next_process]);

    // Stop current process
    kill(pid_array[current_process], SIGSTOP);
    
    // Set current process index to be next process index
    current_process = next_process;
    
    // Start/Continue next process
    kill(pid_array[current_process], SIGCONT);

    return true;
}

void alarm_handler(int signum) 
{
    // Edge case
    if (signum != SIGALRM) 
    { 
        return; 
    } 

    // Schedule next process
    // Returns "if any child is still working" as a bool
    working = schedule_next_process(current_process);
    
    // If all processes are done
    if (working == false) 
    { 
        // Break the infinite loop
        printf("Finished!\n");
        return;
    }

    // Create new alarm
    alarm(QUANTUM);
}

int main(int argc, char *argv[]) 
{
    if (argc != 2) 
    {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *input_file = fopen(argv[1], "r");
    if (input_file == NULL) 
    {
        perror("Error opening input file");
        return 1;
    }
    
    // Register signal handler
    signal(SIGALRM, alarm_handler); 

    // Create an empty sigset_t
    int sig;
    sigset_t sigset;
    sigemptyset(&sigset);
    // Use sigaddset() to add the SIGCONT signal to the set
    sigaddset(&sigset, SIGCONT);
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
            pid_array_working[process_count] = 1;
            process_count++;
        } 
        else if (pid == 0) 
        {
            // Child process
            // Wait for the signal
            sigwait(&sigset, &sig);

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

    // Start the first process
    kill(pid_array[current_process], SIGCONT);
    printf("Starting: %d\n", pid_array[current_process]);

    // Start first alarm
    alarm(QUANTUM);

    // Spin to keep parent process awake
    while (working == true)
    {
        //printf("working: %d\n", working);
        sleep(QUANTUM);
    }    

    return 0;
}
