#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>


bool isFound(FILE *fp, char *str){
    char *line = calloc(sizeof(char), 256);
    while(fgets(line, 256*sizeof(char), fp)){
        if(strstr(line, str) != NULL){
            return 1;
        }
    }
    return 0;
}

void search_dir(char* dirpath, int depth, char* str){
    if (depth == 0){
        exit(0);
    }
    struct dirent* dirp;
    struct stat statbuf;
    pid_t pid;
    if (lstat(dirpath, &statbuf) < 0){
        fprintf(stderr, "Not found\n");
        return;
    }
    if (S_ISDIR(statbuf.st_mode) == 0){
        // not a directory
        char *suffix = strrchr(dirpath, '.');
        if(strcmp(suffix, ".txt")==0){

            FILE *fp = fopen(dirpath, "r");
            if (fp == NULL){
                fprintf(stderr, "Cannot open the file");
                exit(1);
            }
            if (isFound(fp, str)){
                printf("%s\n", dirpath);
                printf("PID %d PPID %d\n", (int)getpid(), (int)getppid());
            }
        }
	return;
    }

    DIR *dp = opendir(dirpath);
    if (dp == NULL){
        fprintf(stderr, "Directory not found\n");
        exit(1);
    }
    while((dirp = readdir(dp)) != NULL){
        struct stat newstat;
        if(strcmp(dirp->d_name, "..") == 0 || strcmp(dirp->d_name, ".") == 0){
            continue;
        }
        char *path = calloc(strlen(dirpath)+strlen(dirp->d_name)+2, sizeof(char));
        strcat(path, dirpath);
        strcat(path, "/");
        strcat(path, dirp->d_name);
        if (lstat(path, &newstat)< 0){
            fprintf(stderr, "File not found");
            return;
        }
        pid = fork();
        if (pid == 0){
            search_dir(path, depth - 1, str);
            exit(0);
        }
        wait(NULL);
    }

    closedir(dp);
}


int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Wrong number of arguments\n");
        exit(1);
    }
    search_dir(argv[1],atoi(argv[2]), argv[3]);
    return 0;
}
