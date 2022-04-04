#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int sig_counter = 0;

void sig_handler(int signum, siginfo_t *info, __attribute__((unused)) void *context){
    int sender_pid = info->si_pid;
    if (signum == SIGUSR1 || signum == SIGRTMIN){
        sig_counter++;
    }
    else if(signum == SIGUSR2){
        if(info->si_code == SI_QUEUE){ // queue
            for (int i = 0; i < sig_counter; ++i) {
                union sigval sigval;
                sigval.sival_int = i;
                sigqueue(sender_pid, SIGUSR1, sigval);
            }
            union sigval sigval;
            sigval.sival_int = sig_counter;
            sigqueue(sender_pid, SIGUSR2, sigval);
        }
        else{ //kill
            for (int i = 0; i < sig_counter; ++i) {
                kill(sender_pid, SIGUSR1);
            }
            kill(sender_pid, SIGUSR2);
        }
    }
    else{ //sigrt
        for (int i = 0; i < sig_counter; ++i) {
            kill(sender_pid, SIGRTMIN);
        }
        kill(sender_pid, SIGRTMAX);
    }
}

int main(){
    printf("Catcher - PID: %d\n", getpid());

    //block other signals
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGUSR2);
    sigdelset(&mask, SIGRTMAX);
    sigdelset(&mask, SIGRTMIN);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    struct sigaction action;
    sigemptyset (&action.sa_mask);
    action.sa_sigaction = sig_handler;
    action.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &action, NULL);
    sigaction(SIGUSR2, &action, NULL);
    sigaction(SIGRTMIN, &action, NULL);
    sigaction(SIGRTMAX, &action, NULL);

    //setup sigset for receiving final signals from sender
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR2);
    sigaddset(&sigset, SIGRTMAX);

    //wait for sigusr2 || sigrtmax
    sigsuspend(&sigset);

    printf("RECEIVED: %d\n", sig_counter);

    return 0;
}