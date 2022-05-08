#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>

#define SEG_SIZE 4096
#define OVEN_SIZE 5
#define TABLE_SIZE 5

#define PROJ_ID 'F'
#define OVEN_ID 'O'
#define TABLE_ID 'T'
#define OVEN_NAME "oven"
#define TABLE_NAME "table"
#define OVEN_SEM 0
#define TABLE_SEM 1

#define PREP_TIME 2
#define BAKING_TIME 4
#define DELIVERY_TIME 5
#define RETURN_TIME 4

typedef struct{
    int array[OVEN_SIZE];
    int pizza_count;
    int to_bake_index;
    int to_take_out_index;
} oven_t;

typedef struct{
    int array[TABLE_SIZE];
    int pizza_count;
    int to_place_index;
    int to_deliver_index;
} table_t;

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
} arg;

key_t get_key(char* path, int proj_id);
int create_sem(key_t key);
int get_sem(key_t key);
void init_sem(int sem_id, int sem_num);
int create_shm(key_t key, int size);
int get_shm(key_t key, int size);
void delete_sem(int sem_id);
void delete_shm_seg(int shm_id);
void* attach_shm(int shm_id);
void lock(int sem_id, int sem_num);
void unlock(int sem_id, int sem_num);
int randint(int min, int max);
char* timestamp();