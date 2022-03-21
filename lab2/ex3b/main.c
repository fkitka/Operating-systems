#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <ftw.h>
struct Result {
    unsigned int files;
    unsigned int dirs;
    unsigned int blks;
    unsigned int chrs;
    unsigned int fifos;
    unsigned int slnks;
    unsigned int socks;
} *res;

int *print_info(char *path, struct stat *statbuf, int type_flag, struct FTW *ftw) {
    if (!type_flag || !ftw){

    }
    char *currentDir = calloc(256, sizeof(char));
    currentDir = getcwd(currentDir, 256);
    if (currentDir == NULL){
        fprintf(stderr, "Cannot get current working directory\n");
        exit(1);
    }
    printf("TYPE: ");
    switch(statbuf->st_mode & S_IFMT) {
        case S_IFREG:
            res->files++;
            printf("file");
            break;
        case S_IFDIR:
            res->dirs++;
            printf("dir");
            break;
        case S_IFCHR:
            res->chrs++;
            printf("char dev");
            break;
        case S_IFBLK:
            res->blks++;
            printf("block dev");
            break;
        case S_IFIFO:
            res->fifos++;
            printf("fifo");
            break;
        case S_IFLNK:
            res->slnks++;
            printf("slink");
            break;
        case S_IFSOCK:
            res->socks++;
            printf("sock");
            break;
        default:
            break;
    }

    printf(" PATH: %s/%s",currentDir, path);
    printf(" LINKS: %ld", statbuf->st_nlink);
    printf(" SIZE: %ld \n", statbuf->st_size);
    printf(" ACCESS: %s", ctime(&statbuf->st_atime));
    printf(" MODIFY: %s\n", ctime(&statbuf->st_mtime));
    free(currentDir);
    return 0;
}
int main(int argc, char* argv[]) {
    if (argc < 2){
        fprintf(stderr, "Directory path is needed\n");
        exit(1);
    }
    res = calloc(1, sizeof(struct Result));
    res->socks = 0;
    res->slnks = 0;
    res->blks = 0;
    res->chrs = 0;
    res->fifos = 0;
    res->dirs = 0;
    res->files = 0;
    nftw(argv[1], (__nftw_func_t) print_info, 1,0);

    printf("SUMMARY: ");
    printf("files %d  directories %d  fifos %d  char devs %d  block devs %d  slinks %d  socks %d\n",
           res->files,
           res->dirs,
           res->fifos,
           res->chrs,
           res->blks,
           res->slnks,
           res->socks);
    free(res);
    return 0;
}
