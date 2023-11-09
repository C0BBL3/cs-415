#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_parser.h"

#define _GUN_SOURCE

int count_token(char* buf, const char* delim)
{
	//TODO：
	/*
	*	#1.	Check for NULL string
	*	#2.	iterate through string counting tokens
	*		Cases to watchout for
	*			a.	string start with delimeter
	*			b. 	string end with delimeter
	*			c.	account NULL for the last token
	*	#3. return the number of token (note not number of delimeter)
	*/
	if (buf == NULL || delim == NULL) {
		return 0;
	}
	
	int count = 0;
	char* saveptr = NULL;
	char* buf2 = strdup(buf);
	char* token = strtok_r(buf2, delim, &saveptr);

	while (token != NULL) {
		count++;
		token = strtok_r(NULL, delim, &saveptr);
	}

	count++;

	free(buf2);

	return count;
}

command_line str_filler(char* buf, const char* delim)
{
	//TODO：
	/*
	*	#1.create command_line variable to be filled and returned
	*	#2.count the number of tokens with count_token function, set num_token. 
    *           one can use strtok_r to remove the \n at the end of the line.
	*	#3.malloc memory for token array inside command_line variable based on the number of tokens.
	*	#4.use function strtok_r to find out the tokens 
    *   	#5.malloc each index of the array with the length of tokens, fill command_list array with tokens, and fill last spot with NULL.
	*	#6. return the variable.
	*/
	command_line command;
	command.num_token = 0;
	command.command_list = NULL;
	
	if (buf == NULL || delim == NULL) {
		return command;
	}

	size_t len = strlen(buf);
	if (len > 0 && buf[len - 1] == '\n') {
		buf[len - 1] = '\0';
	}

	command.num_token = count_token(buf, delim);

	if (command.num_token > 0) {
		command.command_list = (char**)malloc((command.num_token + 1) * sizeof(char*));

		if (command.command_list == NULL) {
			command.num_token = 0;
			return command;
		}

		char* saveptr = NULL;
		char* buf2 = strdup(buf);
		char* token = strtok_r(buf2, delim, &saveptr);
		int i = 0;

		while (token != NULL) {
			int token_length = strlen(token);
			command.command_list[i] = (char*)malloc((token_length + 1) * sizeof(char));

			if (command.command_list[i] == NULL) {
				for (int j = 0; j < i; j++) {
					free(command.command_list[j]);
				}
				free(command.command_list);
				command.num_token = 0;
				return command;
			}

			strcpy(command.command_list[i], token);
			i++;

			token = strtok_r(NULL, delim, &saveptr);
		}

		
		command.command_list[i] = NULL;

		free(token);
		free(buf2);
	}


	return command;
}


void free_command_line(command_line* command)
{
	//TODO：
	/*
	*	#1.	free the array base num_token
	*/
	if (command == NULL) {
		return;
	} 
	
	if(command->command_list != NULL) {
		for (int i = 0; i < command->num_token; i++) {
			free(command->command_list[i]);
		}
	}

	free(command->command_list);


	command->num_token = 0;
	command->command_list = NULL;
}