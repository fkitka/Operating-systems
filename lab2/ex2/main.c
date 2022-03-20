#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/times.h>

long double get_time(clock_t start, clock_t end){
    return (long double)(end-start)/ sysconf(_SC_CLK_TCK);
}
void print_times(clock_t clock_start, clock_t clock_end, struct tms tms_start, struct tms tms_end, FILE* f){
    printf("real time: %Lf\n", get_time(clock_start, clock_end));
    printf("user time: %Lf\n", get_time(tms_start.tms_utime, tms_end.tms_utime));
    printf("sys time: %Lf\n", get_time(tms_start.tms_stime, tms_end.tms_stime));
    fprintf(f,"real time: %Lf\n", get_time(clock_start, clock_end));
    fprintf(f, "user time: %Lf\n", get_time(tms_start.tms_utime, tms_end.tms_utime));
    fprintf(f,"sys time: %Lf\n", get_time(tms_start.tms_stime, tms_end.tms_stime));
    fflush(f);
}

void libFindCharacter(char toFind, char filename[50]) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL){
        fprintf(stderr, "Cannot open the file");
        exit(1);
    }
    char c;
    int foundInLine = 0;
    int numFound = 0;
    int linesFound = 0;
    while(fread(&c, sizeof(char), 1,fp) == 1){
         if (toFind == c){
             numFound++;
             foundInLine = 1;
         }
         else if (c == '\n' && foundInLine){
             linesFound++;
             foundInLine = 0;
         }
    }
    printf("LIB c: %d, l: %d\n", numFound, linesFound);
    fclose(fp);
}

void sysFindCharacter(char toFind, char filename[50]) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0){
        fprintf(stderr, "Cannot open the file");
        exit(1);
    }
    char c;
    int foundInLine = 0;
    int numFound = 0;
    int linesFound = 0;
    while(read(fd, &c, sizeof(char)) == 1){
        if (toFind == c){
            numFound++;
            foundInLine = 1;
        }
        else if (c == '\n' && foundInLine){
            linesFound++;
            foundInLine = 0;
        }
    }
    printf("SYS c: %d, l: %d\n", numFound, linesFound);
    close(fd);
}



int main(int argc, char* argv[]) {
    clock_t clock_start;
    clock_t clock_end;
    struct tms tms_start;
    struct tms tms_end;

    char toFind;
    if (argc != 3){
        fprintf(stderr, "Wrong number of arguments\n");
        exit(1);
    }
    if (strlen(argv[1]) != 1){
        fprintf(stderr, "First argument must be a character\n");
        exit(1);
    }
    toFind = *argv[1];

    FILE *report = fopen("pomiar_zad_2.txt", "w");
    if (report == NULL){
        fprintf(stderr, "Cannot open the file\n");
        exit(1);
    }
    printf("> lib\n");
    fprintf(report, "---LIB---\n");
    clock_start = times(&tms_start);
    libFindCharacter(toFind, argv[2]);
    clock_end = times(&tms_end);
    print_times(clock_start, clock_end, tms_start, tms_end, report);

    printf("> sys\n");
    fprintf(report, "---SYS---\n");
    clock_start = times(&tms_start);
    sysFindCharacter(toFind, argv[2]);
    clock_end = times(&tms_end);
    print_times(clock_start, clock_end, tms_start, tms_end, report);

    printf("> All done!\n");
    fclose(report);
    return 0;
}

