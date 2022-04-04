#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

int sig_counter = 0;

int parse_argument(char *str) {
    if(strcmp(str, "kill") == 0){
        return 0;
    } else if(strcmp(str, "sigqueue") == 0){
        return 1;
    } else if (strcmp(str, "sigrt") == 0) {
        return 2;
    }
    else return -1;
}
void sig_handler(int signum, siginfo_t *info, __attribute__((unused)) void *context){
    if (signum == SIGUSR1 || signum == SIGRTMIN){
        sig_counter++;
    }
    if (signum == SIGUSR2 && info->si_code == SI_QUEUE) {
        union sigval sigval = info->si_value;
        printf("Sender - sigqueue - receieved from catcher %d\n", sigval.sival_int);
    }
}
int main(int argc, char* argv[]) {
    if (argc != 4){
        fprintf(stderr, "Wrong number of arguments\n");
        fprintf(stderr, "Run as: \n./sender <catcher_pid> <num_of_signals> <kill|sigqueue|sigrt>\n");
        exit(1);
    }
    printf("Sender - PID: %d\n", getpid());

    int catcher_pid = atoi(argv[1]);
    int sig_num = atoi(argv[2]);
    int mode = parse_argument(argv[3]);

    switch(mode){
        case 0: // kill
            for (int i = 0; i < sig_num; ++i) {
                kill(catcher_pid, SIGUSR1);
            }
            kill(catcher_pid, SIGUSR2);

            break;
        case 1: // sigqueue
            for (int i = 0; i < sig_num; ++i) {
                union sigval sigval;
                sigval.sival_int = i;
                sigqueue(catcher_pid, SIGUSR1, sigval);
            }
            union sigval sigval;
            sigval.sival_int = sig_num;
            sigqueue(catcher_pid, SIGUSR2, sigval);
            break;
        case 2: // sigrt
            for (int i = 0; i < sig_num; ++i) {
                kill(catcher_pid, SIGRTMIN);
            }
            kill(catcher_pid, SIGRTMAX);
            break;
        default:
            fprintf(stderr, "Invalid mode\n");
            exit(1);
    }

    struct sigaction act;
    act.sa_sigaction = sig_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;

    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGUSR2, &act, NULL);
    sigaction(SIGRTMIN, &act, NULL);
    sigaction(SIGRTMAX, &act, NULL);

    // block other signals
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGUSR2);
    sigdelset(&mask, SIGRTMIN);
    sigdelset(&mask, SIGRTMAX);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    // setup sigset for receiving signals from catcher
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR2);
    sigaddset(&sigset, SIGRTMAX);

    // wait for sigusr2 || sigrtmax
    sigsuspend(&sigset);

    printf("SEND: %d, RECEIVED: %d\n", sig_num, sig_counter);

    return 0;
}
