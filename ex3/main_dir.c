#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <ftw.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define MAX_FILENAME_LEN 256
struct Result {
    unsigned int files;
    unsigned int dirs;
    unsigned int blks;
    unsigned int chrs;
    unsigned int fifos;
    unsigned int slnks;
    unsigned int socks;
} *res;

void count(struct stat statbuf) {
    switch(statbuf.st_mode & S_IFMT){
        case S_IFREG:
            res->files++;
            break;
        case S_IFDIR:
            res->dirs++;
            break;
        case S_IFSOCK:
            res->socks++;
            break;
        case S_IFLNK:
            res->slnks++;
            break;
        case S_IFBLK:
            res->blks++;
            break;
        case S_IFIFO:
            res->fifos++;
            break;
        case S_IFCHR:
            res->chrs++;
            break;
        default:
            break;
    }
}

void print_info(char *path, struct stat statbuf) {
    char *currentDir = calloc(256, sizeof(char));
    currentDir = getcwd(currentDir, 256);
    if (currentDir == NULL){
        fprintf(stderr, "Cannot get current working directory\n");
        exit(1);
    }
    printf("TYPE: ");
    switch(statbuf.st_mode & S_IFMT) {
        case S_IFREG:
            printf("file");
            break;
        case S_IFDIR:
            printf("dir");
            break;
        case S_IFCHR:
            printf("char dev");
            break;
        case S_IFBLK:
            printf("block dev");
            break;
        case S_IFIFO:
            printf("fifo");
            break;
        case S_IFLNK:
            printf("slink");
            break;
        case S_IFSOCK:
            printf("sock");
            break;
        default:
            break;
    }

    printf(" PATH: %s/%s",currentDir, path);
    printf(" LINKS: %ld", statbuf.st_nlink);
    printf(" SIZE: %ld \n", statbuf.st_size);
    printf(" ACCESS: %s", ctime(&statbuf.st_atime));
    printf(" MODIFY: %s\n", ctime(&statbuf.st_mtime));
    free(currentDir);
}

void search_dir(char* dirpath){
    char *path = calloc(strlen(dirpath)+MAX_FILENAME_LEN, sizeof(char));
    struct dirent* dirp;
    struct stat statbuf;
    if (stat(dirpath, &statbuf) < 0){
        fprintf(stderr, "Not found\n");
        return;
    }
    if (S_ISDIR(statbuf.st_mode) == 0){
        // not a directory
        return;
    }

    strcat(path, dirpath);
    strcat(path, "/");
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
        strcat(path, dirp->d_name);
        if (lstat(path, &newstat)< 0){
            fprintf(stderr, "File not found");
            return;
        }
        print_info(path, newstat);
        count(newstat);
        search_dir(path);
        path[strlen(path)-strlen(dirp->d_name)] = 0;

    }
    closedir(dp);
}

int main(int argc, char* argv[]) {
    if (argc < 2){
        fprintf(stderr, "Directory path is needed\n");
        exit(1);
    }
    res = calloc(1, sizeof(struct Result));
    search_dir(argv[1]);
    printf("SUMMARY: ");
    printf("files %d  directories %d  fifos %d  char devs %d  block devs %d  slinks %d  socks %d\n",
           res->files,
           res->dirs,
           res->fifos,
           res->chrs,
           res->blks,
           res->slnks,
           res->socks);
    return 0;
}
