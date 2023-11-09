int check_if_destinationPath_is_file(char *destinationPath); // 1 is file 0 is dir -1 is error

void fix_destinationPath(char* sourcePath, char** destinationPath);

char* generate_new_desinationPath(char** new_destinationPath, char* destinationPath, char* file_name, char* file_extension, int num_of__copy);