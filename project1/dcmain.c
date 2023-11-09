#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syscall.h>
#include "command.h" 
#include "string_parser.h" 
#include "string_alteration.h"


void execute_command(char* command) {
    // Implement the logic to execute Unix commands here
    // You should use the provided header file to parse the command and its arguments

    command_line large_token_buffer;
    command_line small_token_buffer;

    large_token_buffer = str_filler(command, ";");

    for (int i = 0; large_token_buffer.command_list[i] != NULL; i++)
    {
        //tokenize large buffer
        //smaller token is seperated by " "(space bar)
        int count = count_token(large_token_buffer.command_list[i], " ") - 1;
        if (count < 1) { 
            continue;
        }
        
        small_token_buffer = str_filler(large_token_buffer.command_list[i], " ");
        char* smallCommand = strdup(small_token_buffer.command_list[0]);

        // if (strcmp(smallCommand, "cat") == 0) // prints all files given for 'cat' (just an alternate way of using cat)
        // {
        //     for (int j = 1; small_token_buffer.command_list[j] != NULL; j++)
        //     {
        //         display_file(small_token_buffer.command_list[j]);
        //         printf("\n");
        //     }
        // }
        /*else*/ if (count == 1) {
            if (strcmp(smallCommand, "ls") == 0) 
            {
                list_dir();
            } 
            else if (strcmp(smallCommand, "pwd") == 0) 
            {
                show_current_dir();
            } 
            else if (strcmp(smallCommand, "mkdir") == 0 // commands that need parameters
                    || strcmp(smallCommand, "cd") == 0
                    || strcmp(smallCommand, "cp") == 0
                    || strcmp(smallCommand, "mv") == 0
                    || strcmp(smallCommand, "rm") == 0
                    || strcmp(smallCommand, "cat") == 0) 
            {
                printf("Error! Missing parameters for command: %s\n", smallCommand);
            } else {
                printf("Error! Unrecognized command: %s\n", smallCommand);
            }
        } else {
            if ((strcmp(smallCommand, "ls") == 0
              || strcmp(smallCommand, "pwd") == 0) // commands that use no parameters given any amount parameters
              && count >= 2)
            {
                printf("Error! Unsupported parameters for command: %s\n", smallCommand);
            } 
            else if (strcmp(smallCommand, "mkdir") == 0
                    || strcmp(smallCommand, "cd") == 0
                    || strcmp(smallCommand, "rm") == 0
                    || strcmp(smallCommand, "cat") == 0) 
            {
                if (count> 2) // commands that use 1 parameter given 2+ parameters
                {
                    printf("Error! Unsupported parameters for command: %s\n", smallCommand);
                } 
                else if (count== 2)
                {
                    if (strcmp(smallCommand, "mkdir") == 0) {
                        make_dir(small_token_buffer.command_list[1]);
                    }
                    else if (strcmp(smallCommand, "cd") == 0)
                    {
                        change_dir(small_token_buffer.command_list[1]);
                    }
                    else if (strcmp(smallCommand, "rm") == 0)
                    {
                        remove(small_token_buffer.command_list[1]);
                    }
                    else if (strcmp(smallCommand, "cat") == 0)
                    {
                        display_file(small_token_buffer.command_list[1]);
                        printf("\n");
                    }
                    else
                    {
                        printf("Error! Unrecognized command: %s\n", smallCommand);
                    }
                }
            } 
            else if (strcmp(smallCommand, "cp") == 0
                    || strcmp(smallCommand, "mv") == 0) 
            {
                if (count> 3) // commands that use 2 parameters given 3+ parameters
                {
                    printf("Error! Unsupported parameters for command: %s\n", smallCommand);
                }
                else if (count== 3)
                {
                    if (strcmp(smallCommand, "cp") == 0) {
                        copy_file(small_token_buffer.command_list[1], small_token_buffer.command_list[2]);
                    }
                    else if (strcmp(smallCommand, "mv") == 0)
                    {
                        moveFile(small_token_buffer.command_list[1], small_token_buffer.command_list[2]);
                    }
                    else
                    {
                        printf("Error! Unrecognized command: %s\n", smallCommand);
                    }
                }
            } 
            else 
            {
                printf("Error! Unrecognized command: %s\n", smallCommand);
            }
        }

        free(smallCommand);
        free_command_line(&small_token_buffer);
		memset(&small_token_buffer, 0, 0);
    }

    free_command_line(&large_token_buffer);
	memset (&large_token_buffer, 0, 0);
}

int main(int argc, char* argv[]) {
    if (argc == 3 && strcmp(argv[1], "-f") == 0) {
        // read commands from the file and execute them
        FILE *inputFile = fopen(argv[2], "r");
        FILE *outputFile = freopen("output.txt", "w", stdout);

        if (inputFile == NULL || outputFile == NULL) {
            printf("Error opening input file.\n");
            return 1;
        }

        char* line = NULL;
        size_t len = 0;
        size_t read;
        while ((long)(read = getline(&line, &len, inputFile)) != (long)-1) {
            // remove the newline character at the end of the line
            if (line[read - 1] == '\n') {
                line[read - 1] = '\0';
            }

            // execute the command
            execute_command(line);
        }

        free(line);
        fclose(inputFile);
        fclose(outputFile);

        printf("End of file\nBye Bye!\n");
    } else {
        // interactive mode
        char* line = NULL;
        size_t len = 0;

        while (1) {
            printf(">>>");
            getline(&line, &len, stdin);
            
            // remove the newline character at the end of the line
            if (line[strlen(line) - 1] == '\n') {
                line[strlen(line) - 1] = '\0';
            }

            if (strcmp(line, "exit") == 0) {
                break;
            }

            // execute the command
            execute_command(line);
        }

        free(line);
    }

    return 0;
}