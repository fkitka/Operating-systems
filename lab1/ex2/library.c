#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *TMP_FILE_NAME = NULL;

struct ResultArray* create_table(int size){
    if (size <= 0){
        fprintf(stderr, "Wrong array size\n");
        exit(1);
    }
    struct ResultArray* arr = calloc(1,sizeof(struct ResultArray));
    arr->size = size;
    arr->blocks = (struct Block**)calloc(size, sizeof(struct Block*));
    return arr;
}

void wc_files(char* paths){
    if (strcmp(paths, "")==0){
        fprintf(stderr, "Empty file paths\n");
    }

    char tmp[] = "/tmp/tmp.XXXXXX";
    mkstemp(tmp);
    TMP_FILE_NAME = calloc(strlen(tmp) + 1, sizeof (char));
    strcpy(TMP_FILE_NAME, tmp);
    char* command = calloc(10000, sizeof(char));
    strcat(command, "wc ");
    strcat(command, paths);
    strcat(command, " >> ");
    strcat(command, TMP_FILE_NAME);

    system(command);
    free(command);
}

int get_first_empty_block_index(struct ResultArray* arr){
    for (int i = 0; i < arr->size; i++) {
        if (arr->blocks[i] == NULL) {
            return i;
        }
    }
    fprintf(stderr, "No empty blocks found\n");
    exit(4);
}

long get_file_size(FILE* fp) {
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    return size;
}

int create_block(struct ResultArray* arr){
    FILE *f = fopen(TMP_FILE_NAME, "r");
    if (ferror(f)){
        fprintf(stderr, "Error while opening the file\n");
    }
    int index = get_first_empty_block_index(arr);

    struct Block* block = calloc(1, sizeof(struct Block));
    block->size = get_file_size(f);

    char result_content[100000];
    char buffer[256];
    while(fgets(buffer, sizeof(buffer), f)) {
        strcat(result_content, buffer);
    }
    block->content = result_content;
    arr->blocks[index] = block;

    remove(TMP_FILE_NAME);
    fclose(f);

    return index;
}


void remove_block(int index, struct ResultArray* arr){
    if (index >= arr->size || index < 0){
        fprintf(stderr, "Invalid index");
    }
    free(arr->blocks[index]);
    for (int i = index; i < arr->size-1; i++) {
        arr->blocks[i] = arr->blocks[i+1];
    }
}
void print_array(struct ResultArray* arr){
    for (int i = 0; i < arr->size; i++) {
        if (arr->blocks[i] != NULL) {
            printf("arr->blocks[%d]->content = %s\n", i, arr->blocks[i]->content);
        }
    }
}
