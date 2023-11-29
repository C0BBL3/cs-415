#include "string_parser.h"

int count_token (char* buf, const char* delim)
{
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

	free(buf2);

	return count;
}

command_line str_filler(char* buf, const char* delim)
{
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
			free(command.command_list);
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
	if (command == NULL) {
		return;
	}

	if(command->command_list != NULL) {
		for (int i = 0; i < command->num_token - 1; i++) {
			free(command->command_list[i]);
		}

		free(command->command_list);
	}

	command->num_token = 0;
	command->command_list = NULL;
}