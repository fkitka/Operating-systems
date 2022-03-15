#ifndef LAB1_LIBRARY_H
#define LAB1_LIBRARY_H

#include <bits/types/FILE.h>

struct Block {
    long size;
    char* content;
};
struct ResultArray{
    int size;
    struct Block** blocks;
};

struct ResultArray* create_table(int size);
void wc_files(char* paths);
int create_block(struct ResultArray* arr);
void remove_block(int index, struct ResultArray* arr);
void print_array(struct ResultArray* arr);
#endif //LAB1_LIBRARY_H
