#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/times.h>

#define ROW_SIZE 256

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
}

char* removeEmptyLines(char *content) {
    unsigned int size = strlen(content);
    char *res_content = calloc(size, sizeof(char));
    char *buffer = calloc(ROW_SIZE, sizeof(char));
    int emptyLine = 1;
    for (int i = 0; i < size; ++i) {
        if (content[i] == '\n') {
            if (!emptyLine) {
                strcat(res_content, buffer);
                strcat(res_content, "\n");
            }
            free(buffer);
            buffer = calloc(ROW_SIZE, sizeof(char));
            emptyLine = 1;
            continue;
        }
        else if (!isspace(content[i])){
            emptyLine = 0;
        }
        strncat(buffer, &content[i], 1);
    }
    if (emptyLine == 0){
        strcat(res_content, buffer);
    }
    free(buffer);
    return res_content;
}

char* libGetFileContent(char* filename){
    FILE *fp;
    fp = fopen(filename, "r");
    if(fp == NULL){
        fprintf(stderr, "Cannot open the file");
        exit(1);
    }
    // file size
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *buffer = calloc(size, sizeof(char));
    unsigned int numread = fread(buffer, sizeof(char), size, fp);
    if (ferror(fp) || numread < size){
        fprintf(stderr, "Cannot read the file");
        exit(1);
    }
    fclose(fp);
    buffer = removeEmptyLines(buffer);
    return buffer;
}

void libPutContentToFile(char* content, char* filename){
    FILE* fp = fopen(filename, "w");
    if (fp == NULL){
        fprintf(stderr, "Cannot open the file");
        exit(1);
    }
    fwrite(content, sizeof(char), strlen(content), fp);
    if(ferror(fp)){
        fprintf(stderr, "Cannot write to the file");
        exit(1);
    }
    fclose(fp);
}

char* sysGetFileContent(char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0){
        fprintf(stderr, "Cannot open the file");
        exit(1);
    }
    // file size
    struct stat st;
    fstat(fd, &st);
    unsigned int size = st.st_size;

    char *buffer = calloc(size, sizeof(char));
    long numread = read(fd, buffer, size);
    if (numread < size){
        fprintf(stderr, "Cannot read the file");
        exit(1);
    }
    close(fd);
    buffer = removeEmptyLines(buffer);
    return buffer;
}

void sysPutContentToFile(char *content, char *filename) {
    int fd = open(filename, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd < 0){
        fprintf(stderr, "Cannot open the file");
        exit(1);
    }
    unsigned long size = strlen(content);
    long numwritten = write(fd, content, size);
    if(numwritten < size) {
        fprintf(stderr, "Cannot write to the file");
        exit(1);
    }
    close(fd);
}

int main(int argc, char* argv[]) {
    clock_t clock_start;
    clock_t clock_end;
    struct tms tms_start;
    struct tms tms_end;
    FILE *report = fopen("pomiar_zad_1.txt", "w");

    char *first_file = calloc(50, sizeof(char));
    char *second_file = calloc(50, sizeof(char));
    if (argc < 2){
        printf(">> first file: ");
        scanf("%s", first_file);
        printf(">> second file: ");
        scanf("%s", second_file);
    }
    else if (argc == 3){
        strcpy(first_file, argv[1]);
        strcpy(second_file, argv[2]);
    }
    else{
        fprintf(stderr, "Not enough arguments");
        exit(1);
    }
    char* content;
    printf(">lib\n");
    fprintf(report, "---LIB---\n");
    clock_start = times(&tms_start);
    content = libGetFileContent(first_file);
    libPutContentToFile(content, second_file);
    clock_end = times(&tms_end);
    print_times(clock_start, clock_end, tms_start, tms_end, report);
    printf(">sys\n");
    fprintf(report, "---SYS---\n");
    clock_start = times(&tms_start);
    content = sysGetFileContent(first_file);
    sysPutContentToFile(content, second_file);
    clock_end = times(&tms_end);
    print_times(clock_start, clock_end, tms_start, tms_end, report);
    printf(">All done!\n");
    fclose(report);
    return 0;
}
