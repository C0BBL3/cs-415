#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "string_parser.h"

int check_if_destinationPath_is_file(char *path) { // 1 is file 0 is dir -1 is error
    if (strcmp(path, "./") == 0) { return 0; } // edge case bc the below code detects this as a erro
    if (strcmp(path, "../") == 0) { return 0; } // edge case bc the below code detects this as a erro

    command_line token_buffer = str_filler(path, "/"); // parses the destination path
    int count = count_token(token_buffer.command_list[token_buffer.num_token - 2], ".") - 1; // split final token with "."
    
    free_command_line(&token_buffer);
	memset(&token_buffer, 0, 0);

    if (count == 2) { // checks if final token is a file, dir, or error in that order
        return 1;
    } else if (count == 1) {
        return 0;
    }
    
    return -1;
}

char* generate_new_desinationPath(char* new_destinationPath, char* destinationPath, char* file_name, char* file_extension, int num_of__copy) {
    size_t new_destinationPathLength = strlen(destinationPath) + strlen(file_name) + strlen(file_extension) + 4 + 5 * num_of__copy;
    new_destinationPath = (char*)realloc(new_destinationPath, new_destinationPathLength * sizeof(char));

    strcpy(new_destinationPath, destinationPath);
    strcat(new_destinationPath, file_name);
    
    if (num_of__copy > 0){
        for (int i = 0; i < num_of__copy; i++) {
            strcat(new_destinationPath, "_copy");
        }
    }

    strcat(new_destinationPath, ".");
    strcat(new_destinationPath, file_extension);

    return new_destinationPath;
}

void fix_destinationPath(char* sourcePath, char** destinationPathPtr) {
    command_line source_token_buffer = str_filler(sourcePath, "/"); // parses the source path
    command_line final_source_token_buffer = str_filler(source_token_buffer.command_list[source_token_buffer.num_token - 2], ".");

    free_command_line(&source_token_buffer);
	memset(&source_token_buffer, 0, 0);
    
    if (final_source_token_buffer.num_token == 1) { // edge case
        return;
    }

    char* file_name = (char*)final_source_token_buffer.command_list[0];
    char* file_extension = (char*)final_source_token_buffer.command_list[1]; // not including the "."

    char* new_destinationPath = (char*)malloc((strlen(*destinationPathPtr) + strlen(file_name) + strlen(file_extension) + 4) * sizeof(char));
    new_destinationPath = generate_new_desinationPath(new_destinationPath, *destinationPathPtr, file_name, file_extension, 0);

    if (access(new_destinationPath, F_OK) == 0) {
        int num_of__copy = 1;
        new_destinationPath = generate_new_desinationPath(new_destinationPath, *destinationPathPtr, file_name, file_extension, num_of__copy);

        while(access(new_destinationPath, F_OK) == 0) { // if the _copy exists keep adding _copy's until it that file doesnt exist
            num_of__copy += 1;
            new_destinationPath = generate_new_desinationPath(new_destinationPath, *destinationPathPtr, file_name, file_extension, num_of__copy);
        }
    }

    //cp files/newerfiles/input.txt files/newerfiles/

    free_command_line(&final_source_token_buffer);
	memset (&final_source_token_buffer, 0, 0);

    strcpy(*destinationPathPtr, new_destinationPath);
    free(new_destinationPath);
}