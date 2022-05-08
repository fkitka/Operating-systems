#include "common.h"

key_t get_key(char *path, int proj_id)
{
    key_t key = ftok(path, proj_id);
    if (key == -1)
    {
        fprintf(stderr, "Cannot generate a key\n");
        exit(1);
    }
    return key;
}
int create_sem(key_t key)
{
    int sem_id = semget(key, 5, 0666 | IPC_CREAT);
    if (sem_id == -1)
    {
        fprintf(stderr, "Cannot create semaphors\n");
        exit(1);
    }
    return sem_id;
}

void init_sem(int sem_id, int sem_num)
{
    union arg;
    arg.val = 1;
    if (semctl(sem_id, sem_num, SETVAL, arg) == -1)
    {
        fprintf(stderr, "Cannot init semaphore value\n");
        exit(1);
    }
}

int create_shm(key_t key, int size)
{
    int shm_id = shmget(key, size, IPC_CREAT | 0666);
    if (shm_id == -1)
    {
        fprintf(stderr, "Cannot create shared memory\n");
        exit(1);
    }
    return shm_id;
}

void *attach_shm(int shm_id)
{
    void *res;
    if ((res = shmat(shm_id, NULL, 0)) == (void *)-1)
    {
        fprintf(stderr, "Cannot attach shared memory\n");
        exit(1);
    }
    return res;
}

void delete_sem(int sem_id)
{
    semctl(sem_id, 0, IPC_RMID, NULL);
}

void delete_shm_seg(int shm_id)
{
    shmctl(shm_id, IPC_RMID, NULL);
}

int get_sem(key_t key){
    int sem_id = semget(key, 5, 0);
    if (sem_id == -1)
    {
        fprintf(stderr, "Cannot create semaphors\n");
        exit(1);
    }
    return sem_id;
}

int get_shm(key_t key, int size){
    int shm_id = shmget(key, size, 0);
    if (shm_id == -1)
    {
        fprintf(stderr, "Cannot create shared memory\n");
        exit(1);
    }
    return shm_id;
}

void lock(int sem_id, int sem_num){
    struct sembuf sb;
    sb.sem_op = -1;
    sb.sem_num = sem_num;
    sb.sem_flg = 0;
    if(semop(sem_id, &sb, 1) == -1){
        fprintf(stderr, "Cannot lock the semaphore\n");
        exit(1);
    }
}

void unlock(int sem_id, int sem_num){
struct sembuf sb;
    sb.sem_op = 1;
    sb.sem_num = sem_num;
    sb.sem_flg = 0;
    if(semop(sem_id, &sb, 1) == -1){
        fprintf(stderr, "Cannot unlock the semaphore\n");
        exit(1);
    }
}

int randint(int min, int max){
    return (rand() % (max-min)) + min;
}

char* timestamp(){
    char *result_time = calloc(140, sizeof(char));
    char time_str[128];
    int milliseconds;
    double fractional_seconds;
    struct timeval tv;

    gettimeofday(&tv, NULL);

    struct tm* tm = localtime(&tv.tv_sec);

    fractional_seconds = (double) tv.tv_usec;
    fractional_seconds /= 1e3;
    milliseconds = (int) fractional_seconds;

    strftime(time_str, sizeof(time_str), "%Y-%m-%dT%H:%M:%S", tm);
    sprintf(result_time, "%s.%03d", time_str, milliseconds);

    return result_time;    
}