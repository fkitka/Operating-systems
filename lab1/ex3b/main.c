#include "library.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/times.h>
#include <string.h>
#include <stdlib.h>

#ifdef DYNAMIC
#include <dlfcn.h>
#endif

void free_table(struct ResultArray* arr){
    for (int i = 0; i < arr->size; ++i) {
        free(arr->blocks[i]);
    }
    free(arr->blocks);
    free(arr);
}

int is_filename(char* arg){
    if (strcmp(arg, "create_table") == 0 ||
    strcmp(arg, "wc_files") == 0 ||
    strcmp(arg, "remove_block") == 0){
        return 0;
    }
    return 1;
}
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
int main(int argc, char *argv[]){
    if (argc <= 1){
        fprintf(stderr, "Arguments expected\n");
        exit(1);
    }
    #ifdef DYNAMIC
        void *handle = dlopen("liblibrary.so", RTLD_LAZY);
        if(!handle){
            fprintf(stderr, "Cannot load library\n");
            exit(1);
        }
        struct ResultArray* (*create_table)(int size) = dlsym(handle, "create_table");
        void (*wc_files)(char* paths) = dlsym(handle, "wc_files");
        int (*create_block)(struct ResultArray* arr) = dlsym(handle, "create_block");
        void (*remove_block)(int index, struct ResultArray* arr) = dlsym(handle, "remove_block");
//        void (*print_array)(struct ResultArray* arr) = dlsym(handle, "print_array");

        if(dlerror() != NULL){
            fprintf(stderr, "Cannot load functions from library\n");
            exit(1);
        }
    #endif
    FILE* report_file = fopen("results3b.txt", "a");
    if (ferror(report_file)){
        fprintf(stderr, "Error while opening the file\n");
    }
    clock_t clock_start;
    clock_t clock_end;
    struct tms tms_start;
    struct tms tms_end;
    struct ResultArray *result = NULL;

    for (int i = 1; i < argc; ++i) {
        printf("NEW OPERATION %s\n", argv[i]);
        fprintf(report_file,"--OPERATION %s--\n", argv[i]);
        clock_start = times(&tms_start);
        if(strcmp(argv[i], "create_table") == 0){
            int size = atoi(argv[++i]);
            result = create_table(size);
        }
        else if(strcmp(argv[i], "wc_files") == 0){
            int files_count = 0;
            int files_len = 0;
            char paths[10000];
            for (int j = i+1; j < argc; ++j) {
                if (is_filename(argv[j])){
                    files_count++;
                    files_len += strlen(argv[j]);
                    if (files_count > 0) {strcat(paths, " ");}
                    strcat(paths, argv[j]);
                }
                else{
                    break;
                }
            }
            if(files_count==0){
                fprintf(stderr, "wc_files is expecting arguments");
                exit(1);
            }
            wc_files(paths);
            create_block(result);
//            print_array(result);
            i += files_count;
        }
        else if(strcmp(argv[i], "remove_block") == 0){
            int index = atoi(argv[i++]);
            remove_block(index, result);
//            print_array(result);
        }
        else{
            fprintf(stderr, "Command not found");
            exit(1);
        }
        clock_end = times(&tms_end);
        print_times(clock_start, clock_end, tms_start, tms_end, report_file);
    }
    free_table(result);
    fclose(report_file);

    #ifdef DYNAMIC
        dlclose(handle);
    #endif

    return 0;
}
