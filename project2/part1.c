#include "MCP.h"
#include "string_parser.h"

pid_t pid_array[MAX_PROCESSES]; // Array to store child process IDs
int process_count = 0; // Number of child processes

int main(int argc, char *argv[]) 
{
    // Check command line arguments and open the input file
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s input_file\n", argv[0]);
        exit(1);
    }

    FILE *input_file = fopen(argv[1], "r");
    if (input_file == NULL) 
    {
        perror("Error opening input file");
        exit(1);
    }

    // Read and fork child processes from the input file
    command_line token_buffer;

	// Declare line_buffer
	size_t len = MAX_COMMAND_LENGTH;
	char* line_buf = malloc (MAX_COMMAND_LENGTH);

    while (getline(&line_buf, &len, input_file) != -1 && process_count < MAX_PROCESSES) 
    {
        token_buffer = str_filler(line_buf, " ");

        // Fork a child process
        pid_t child_pid = fork();

        if (child_pid == -1) 
        {
            perror("Fork error");
            exit(1);
        } 
        else if (child_pid == 0) 
        {
            // Child process
            if (execvp(token_buffer.command_list[0], token_buffer.command_list) == -1) 
            {
                // Child had issue executing command so exit
                perror("Exec error");
                exit(1);
            }
        } 
        else 
        {
            // Parent process
            pid_array[process_count] = child_pid;
            process_count++;
        }

        // Free tokens and reset variable
        free_command_line(&token_buffer);
        memset(&token_buffer, 0, 0);
    }

    // Free line buffer
    free(line_buf);

    // Close the input file
    fclose(input_file);

    // Wait for child processes to finish
    for (int i = 0; i < process_count; i++) {
        int status, w;
        w = waitpid(pid_array[i], &status, 0);
        if (w < 0) {
            perror("waitpid");
            // Dont want to return/exit 
            // Because w < 0 when next process is stopped
            // Also it goes to next process so continue is redundant
        }
    }

    fflush(stdout);
    printf("Finished!\n");

    return 0;
}