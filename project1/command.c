#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <dirent.h>
#include <errno.h>

#include "command.h"
#include "string_parser.h"
#include "string_alteration.h"

extern int chdir();
extern int unlink();
extern int readlink();

void list_dir() {
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            printf("%s\t", dir->d_name);
        }
        closedir(d);
    }
    printf("\n");
}

void show_current_dir() {
    DIR *dir;
    char path[1024];

    dir = opendir("/proc/self");

    if (dir == NULL) {
        perror("Error opening directory");
        return;
    }

    size_t n = readlink("/proc/self/cwd", path, sizeof(path) - 1);
    if (n != (long unsigned int)-1) {
        path[n] = '\0';
        printf("%s\n", path);
    } else {
        perror("Error reading symbolic link");
    }

    closedir(dir);
}

void make_dir(char *dirName) {
    if (mkdir(dirName, 0x777) != 0) {
        printf("Directory already exists!\n");
        perror("mkdir");
    } else {
        printf("Directory creation succesful!\n");
    }
}

void change_dir(char *dirName) {
    if (chdir(dirName) != 0) {
        perror("chdir");
    }
}

void copy_file(char *sourcePath, char *destinationPath) {
    FILE * fptr1 = fopen(sourcePath, "r"); 
    if (fptr1 == NULL) 
    { 
        printf("Error! Issue when opening source file!\n"); 
        return;
    }

    if (strcmp(destinationPath, ".") == 0) { 
        strcat(destinationPath, "/");
    } 

    int isDestinationPathFile = check_if_destinationPath_is_file(destinationPath); // 1, 0 or -1

    if (isDestinationPathFile == 0) {
        fix_destinationPath(sourcePath, &destinationPath);
    
        if (destinationPath == NULL) {
            isDestinationPathFile = -1;
        }
    }
    
    if (isDestinationPathFile == -1)
    { 
        printf("Error! Issue when creating destination file! Please try again with a new paste directory!\n");
        fclose(fptr1);
        return;
    }

    FILE *fptr2 = fopen(destinationPath, "w");
    if (fptr2 == NULL) // edge case
    { 
        printf("Error! Issue when creating destination file!\n");
        fclose(fptr1);
        return;
    } 

    char* c = (char*)malloc(10 * sizeof(char));
    *c = fgetc(fptr1); 
    while (*c != EOF)
    { 
        fputc(*c, fptr2); 
        *c = fgetc(fptr1); 
    }

    fclose(fptr1);
    fclose(fptr2);
    free(c);
}

void moveFile(char *sourcePath, char *destinationPath) {
    copy_file(sourcePath, destinationPath);
    delete_file(sourcePath);
}

void delete_file(char *filename) {
    if (unlink(filename) != 0) {
        perror("unlink");
    }
}

void display_file(char *filename) {
    FILE *fptr;

    fptr = fopen(filename, "r"); 
    if (fptr == NULL) 
    { 
        printf("Error! Issue when opening source file!\n"); 
        return;
    } 

    size_t len = 1024;
	char* line_buf = NULL;

    while (getline (&line_buf, &len, fptr) != -1)
	{
		printf("%s", line_buf);
	}

	fclose(fptr);
	free(line_buf);
}
