#include "common.h"

int shm_oven_id, shm_table_id, sem_id;

void create_sems();
void create_shm_seg();
void sigint_handler();
void init_oven(oven_t* oven);
void init_table(table_t* table);

int main(int argc, char *argv[]){
    if (argc != 3){
        fprintf(stderr, "Wrong number of arguments\n");
        exit(1);
    }
    signal(SIGINT, sigint_handler);

    int N = atoi(argv[1]);
    int M = atoi(argv[2]);

    create_sems();
    create_shm_seg();

    for (int i = 0; i < N; i++){
        pid_t pid = fork();
        if (pid == 0){
            execl("./cook", "./cook", NULL);
        }
    }
    for(int i = 0; i < M; i++){
        pid_t pid = fork();
        if(pid == 0){
            execl("./supplier", "./supplier", NULL);
        }
    }
    for(int i = 0; i < N + M; i++){
        wait(NULL);
    }
    return 0;
}

void init_oven(oven_t* oven){
    for (int i = 0; i < OVEN_SIZE; i++){
        oven->array[i] = -1;
    }
    oven->pizza_count = 0;
    oven->to_bake_index = 0;
    oven->to_take_out_index = 0;
}

void init_table(table_t* table){
    for (int i = 0; i < TABLE_SIZE; i++){
        table->array[i] = -1;
    }
    table->pizza_count = 0;
    table->to_place_index = 0;
    table->to_deliver_index = 0;
}

void create_sems(){
    key_t key = get_key(getenv("HOME"), PROJ_ID);
    sem_id = create_sem(key);
    init_sem(sem_id, OVEN_SEM);
    init_sem(sem_id, TABLE_SEM);
    printf("Semaphore id: %d\n", sem_id);
}

void create_shm_seg(){
    key_t oven_key = get_key(OVEN_NAME, OVEN_ID);
    key_t table_key = get_key(TABLE_NAME, TABLE_ID);
    shm_oven_id = create_shm(oven_key, SEG_SIZE);
    shm_table_id = create_shm(table_key, SEG_SIZE);
    oven_t *oven = attach_shm(shm_oven_id);
    table_t *table = attach_shm(shm_oven_id);
    init_oven(oven);
    init_table(table);
    printf("Shared memory: oven %d table %d\n",shm_oven_id, shm_table_id);
}

void sigint_handler(int signum){
    delete_sem(sem_id);
    delete_shm_seg(shm_oven_id);
    delete_shm_seg(shm_table_id);
    printf("\nSemaphores and shared memory deleted\n");
}