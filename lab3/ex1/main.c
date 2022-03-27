#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    if (argc != 2){
        fprintf(stderr, "Wrong number of arguments\n");
        exit(1);
    }
    int n = atoi(argv[1]);
    pid_t pid;
    printf("PID glownego programu : %d\n", (int)getpid());
    for (int i = 0; i < n; i++){
        pid = fork();
        if (pid > 0){
            // PARENT
        }
        if (pid == 0){
            // CHILD
            printf("IM A CHILD - PID %d, PPID %d\n", (int)getpid(), (int)getppid());
            exit(0);
        }
    }
    for(int i = 0; i<n; i++){
        wait(NULL);
    }



    return 0;
}