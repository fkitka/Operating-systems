#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <string.h>

long double get_time(clock_t start, clock_t end){
    return (long double)(end-start)/ sysconf(_SC_CLK_TCK);
}
void print_times(clock_t clock_start, clock_t clock_end, struct tms tms_start, struct tms tms_end, FILE* f){
//    printf("real time: %Lf\n", get_time(clock_start, clock_end));
//    printf("user time: %Lf\n", get_time(tms_start.tms_utime, tms_end.tms_utime));
//    printf("sys time: %Lf\n", get_time(tms_start.tms_stime, tms_end.tms_stime));
    fprintf(f,"real time: %Lf\n", get_time(clock_start, clock_end));
    fprintf(f, "user time: %Lf\n", get_time(tms_start.tms_utime, tms_end.tms_utime));
    fprintf(f,"sys time: %Lf\n", get_time(tms_start.tms_stime, tms_end.tms_stime));
}

void print_to_file(int i, double result) {
    char* filename = calloc(10, sizeof(char));
    sprintf(filename, "w%d.txt", i);
    FILE *fp = fopen(filename, "w");
    fprintf(fp, "%f", result);
//    printf("Printing result: %f, file w%d.txt...\n", result, i);
    free(filename);
    fclose(fp);
}

double f(double x){
    return 4/(x*x+1);
}

double getRectArea(double x1, double x2) {
    return f(x2) * (x2-x1);
}

void calc(int i, int n, double width) {
    double result = 0;
    double x = (double)i / (double)n;
    double right_limit = (double)(i+1)/ (double)n;
    while (x + width <= right_limit){
        result += getRectArea(x, x+width);
        right_limit -= width;
    }
    result += getRectArea(x, right_limit);
    print_to_file(i+1, result);
}

long getSize(FILE *fp) {
    long size;
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    return size;
}

double getResult(FILE *fp, long size) {
    char* str = calloc(size+1, sizeof(char));
    fread(str, size, 1, fp);
    double result = atof(str);
    free(str);
    return result;
}

int main(int argc, char* argv[]) {
    if (argc != 3){
        fprintf(stderr, "Wrong number of arguments");
        exit(1);
    }
    clock_t clock_start;
    clock_t clock_end;
    struct tms tms_start;
    struct tms tms_end;
    FILE *report = fopen("report.txt", "a");
    double width = atof(argv[1]);
    int n = atoi(argv[2]);
    double result = 0;
    pid_t pid;
    clock_start = times(&tms_start);
    for (int i = 0; i < n; i++){
        pid = fork();
        if (pid == 0){
            calc(i, n, width);
            exit(0);
        }
    }
    for (int i = 0; i < n; i++){
        wait(NULL);
    }
    for (int i = 1; i < n+1; i++){
        char *filename = calloc(10, sizeof(char));
        sprintf(filename, "w%d.txt", i);
        FILE* fp = fopen(filename, "r");
        long size = getSize(fp);
        double n_result = getResult(fp, size);
        result += n_result;
        fclose(fp);
    }
    clock_end = times(&tms_end);
    fprintf(report, "Number of processes: %d, rectangle width: %lf \n > RESULT: %lf\n", n, width, result);
    print_times(clock_start, clock_end, tms_start, tms_end, report);
    printf("INTEGRATION RESULT: %f\n", result);
    fclose(report);
    return 0;
}


