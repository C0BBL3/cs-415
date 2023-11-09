#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

void lfcat()
{
        /* Get the current directory with getcwd() */
        char current_directory[1024];
        if (getcwd(current_directory, sizeof(current_directory)) == NULL) {
                perror("getcwd");
                return;
        }

        /* Open the directory using opendir() */
        DIR *dir = opendir(current_directory);
        if (dir == NULL) {
                perror("opendir");
                return;
        }

        struct dirent *entry;

        /* use a while loop to read the dir with readdir() */
        while ((entry = readdir(dir)) != NULL) {
                char *filename = entry->d_name;

                /* Skip any names that are not readable files */
                if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0 ||
                        strcmp(filename, "main.c") == 0 || strcmp(filename, "lab2.exe") == 0 ||
                        strcmp(filename, "output.txt") == 0) {
                        continue;
                }

                /* Open the file */
                char filepath[1024];
                snprintf(filepath, sizeof(filepath), "%s/%s", current_directory, filename);
                FILE *file = fopen(filepath, "r");

                if (file == NULL) {
                        perror("fopen");
                        continue;
                }

                /* Read and print each line using getline() */
                char *line = NULL;
                size_t len = 0;
                ssize_t read;

                while ((read = getline(&line, &len, file)) != -1) {
                        /* Write the line to stdout */
                        fwrite(line, 1, read, stdout);
                }

                /* Write 80 "-" characters to stdout */
                for (int i = 0; i < 80; i++) {
                        putchar('-');
                }

                /* Close the file and free/null assign your line buffer */
                fclose(file);
                if (line) {
                        free(line);
                        line = NULL;
                }
        }

        /* Close the directory you were reading from using closedir() */
        closedir(dir);
}
