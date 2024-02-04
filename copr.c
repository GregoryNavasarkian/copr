#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>


void printHelp();
void copy(const char* arg, const char* cwd);
void paste();
int isDir(const char* path);
void clearClipboard();


int main(int argc, char *argv[]) {

    char cwd[PATH_MAX];
    
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd() error");
        return 1;
    }
    if (argc < 2 || argc > 3) {
        printf("Copr: invalid arguments\n");
        printf("For help: copr -h\n");
        return 0;
    }
    
    if (argc == 2) {

        if (strcmp(argv[1], "-c") == 0) {
            copy("null", cwd);
        }
        else if (strcmp(argv[1], "-p") == 0) {
            paste();
        }
        else if (strcmp(argv[1], "-h") == 0) {
            printHelp();
        }
        else if (strcmp(argv[1], "-r") == 0) {
            clearClipboard();
            printf("Copr: clipboard cleared\n");
        }
        else {
            printf("Copr: unknown arg\n");
        }
    } 
    else if (argc == 3) {

        if (strcmp(argv[1], "-c") == 0) {
            copy(argv[2], cwd);
        }
        else if (strcmp(argv[1], "-p") == 0) {
            printf("Copr: too many args\n"); 
        }
        else if (strcmp(argv[1], "-h") == 0) {
            printHelp();
        }
        else if (strcmp(argv[1], "-r") == 0) {
            clearClipboard();
            printf("Copr: clipboard cleared\n");
        }
        else {
            printf("Copr: unknown arg\n");
        }
    }
    return 0;
}


void printHelp() {
    printf("Copr: usage: copr [-c] [-p] [-r] [file]\n");
    printf("Copr: -c: copy file or current directory\n");
    printf("Copr: -p: paste file\n");
    printf("Copr: -r: remove file from clipboard\n");
}


void copy(const char* arg, const char* cwd) {
    // var to hold full path 
    char path[PATH_MAX];

    // if arg is none then copy current directory
    if (strcmp(arg, "null") == 0) {
        strcpy(path, cwd);
        printf("Copr copied: %s\n", path);
    }
    // else copy path and add filename
    else {
        strcpy(path, cwd);
        strcat(path, "/");
        strcat(path, arg);
        if (access(path, F_OK) != 0) {
            printf("Copr: file does not exist\n");
            exit(1);
        }
        printf("Copr copied: %s\n", path);
    }
    // remove file if it exists
    clearClipboard();
    // open file and write path
    FILE *file;
    file = fopen("/tmp/copr.txt", "w");
    if (file == NULL) {
        printf("copr: error copying\n");
        exit(1);
    }
    fprintf(file, path);
    fclose(file);
}

// returns 1 if arg is a directory else 0
int isDir(const char* path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0) {
        return 0;
    }
    return S_ISDIR(statbuf.st_mode);
}


void paste() {
    // var to hold path from file
    char path[PATH_MAX];
    // open file and read path
    FILE *file;
    file = fopen("/tmp/copr.txt", "r");
    if (file == NULL) {
        printf("Copr: Nothing in clipboard\n");
        exit(1);
    }
    fscanf(file, "%s", path);
    fclose(file);
    
    // check if file or dir
    bool dir = isDir(path);
    // copy file or dir to current directory
    if (dir == 1) {
        char *cmd = malloc(strlen("cp -r ") + strlen(path) + strlen(" .") + 1);
        strcpy(cmd, "cp -r ");
        strcat(cmd, path);
        strcat(cmd, " .");
        system(cmd);
        free(cmd);
    }
    else {
        char *cmd = malloc(strlen("cp ") + strlen(path) + strlen(" .") + 1);
        strcpy(cmd, "cp ");
        strcat(cmd, path);
        strcat(cmd, " .");
        system(cmd);
        free(cmd);
    }
    printf("Copr pasted: %s\n", path);
}

void clearClipboard() {
    remove("/tmp/copr.txt");
}
