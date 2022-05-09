#include "common.h"

void create_sem(char* name)
{
    sem_t *sem = sem_open(name, O_CREAT, 0666, 1);
    if (sem == NULL)
    {
        fprintf(stderr, "Cannot create semaphors\n");
        exit(1);
    }
}


int create_shm(char* name, int size)
{
    int shm_fd = shm_open(name, O_RDWR | O_CREAT, 0666);
    if (shm_fd == -1)
    {
        fprintf(stderr, "Cannot create shared memory\n");
        exit(1);
    }
    if (ftruncate(shm_fd, size) == -1){
        fprintf(stderr, "Cannot set size of shared memory\n");
        exit(1);
    }
    return shm_fd;
}

void *attach_shm(int shm_fd)
{
    void *res;
    if ((res = mmap(NULL, SEG_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd,0)) == (void *)-1)
    {
        fprintf(stderr, "Cannot attach shared memory\n");
        exit(1);
    }
    return res;
}

void delete_sem(char* name)
{
    sem_unlink(name);
}

void delete_shm_seg(char* name)
{
    shm_unlink(name);
}

sem_t* get_sem(char* name){
    sem_t* sem = sem_open(name, O_RDWR);
    if (sem == SEM_FAILED)
    {
        fprintf(stderr, "Cannot create semaphors\n");
        exit(1);
    }
    return sem;
}

int get_shm(char* name){
    int shm_fd= shm_open(name, O_RDWR, 0666);
    if (shm_fd == -1)
    {
        fprintf(stderr, "Cannot create shared memory\n");
        exit(1);
    }
    return shm_fd;
}

void lock(sem_t *sem){
    if(sem_wait(sem) == -1){
        fprintf(stderr, "Cannot lock semaphore\n");
        exit(1);
    }
}

void unlock(sem_t *sem){
    if(sem_post(sem) == -1){
        fprintf(stderr, "Cannot unlock semaphore\n");
        exit(1);
    }
}

int randint(int min, int max){
    return (rand() % (max-min + 1)) + min;
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