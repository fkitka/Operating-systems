#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <sys/mman.h>

#define SEG_SIZE 4096
#define OVEN_SIZE 5
#define TABLE_SIZE 5

#define PROJ_ID 'F'
#define OVEN_ID 'O'
#define TABLE_ID 'T'
#define OVEN_NAME "/oven_shm"
#define TABLE_NAME "/table_shm"
#define OVEN_SEM "/oven_sem"
#define TABLE_SEM "/table_sem"

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

void create_sem(char* name);

sem_t* get_sem(char* name);

int create_shm(char* name, int size);

int get_shm(char* name);

void delete_sem(char* name);

void delete_shm_seg(char* name);

void* attach_shm(int shm_fd);

void lock(sem_t *sem);

void unlock(sem_t *sem);

int randint(int min, int max);

char* timestamp();