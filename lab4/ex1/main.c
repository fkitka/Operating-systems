#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#define SIGNAL SIGUSR1

int parse_argument(char *str) {
    if (strcmp("ignore", str) == 0){
        return 0;
    }
    else if(strcmp("handler", str) == 0){
        return 1;
    }
    else if(strcmp("mask", str) == 0){
        return 2;
    }
    else if(strcmp("pending", str) == 0){
        return 3;
    }
    else
        return -1;
}

void handle_signal(int signum){
    printf("Recieved signal: %d\n", signum);
}
void check_is_pending(){
    sigset_t pending;
    sigpending(&pending);
    if(sigismember(&pending, SIGNAL)){
        printf("Signal is pending\n");
    }
    else printf("Signal is not pending\n");
}

int main(int argc, char* argv[]){
    if (argc != 2) {
        fprintf(stderr, "Wrong number of arguments\n");
        fprintf(stderr, "Run as: \n ./main <ignore|handler|mask|pending>\n");
        exit(1);
    }

    int action = parse_argument(argv[1]);
    switch(action){
        case 0:
            signal(SIGNAL, SIG_IGN);
            break;
        case 1:
            signal(SIGNAL, handle_signal);
            break;
        case 2:
        case 3:{
            sigset_t newmask;
            sigset_t oldmask;
            sigemptyset(&newmask);
            sigaddset(&newmask, SIGNAL);
            if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
                fprintf(stderr, "Cannot block the signal\n");
        }
            break;
        default:
            fprintf(stderr,"Argument is invalid\n");
            exit(1);
    }
    printf("Parent - raising signal...\n");
    raise(SIGNAL);
    printf("Parent - ");
    check_is_pending();
    int pid = fork();
    if (pid == 0){
        if (action != 3){
            printf("Child - raising signal...\n");
            raise(SIGNAL);
        }
        printf("Child - ");
        check_is_pending();
        if (execl("./exec", "exec", NULL) == -1){
            fprintf(stderr, "Cannot find a file to execute");
            exit(1);
        }
    }
    else{
        wait(NULL);
    }

    return 0;
}
