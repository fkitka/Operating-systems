#include <stdio.h>
#include <signal.h>
#define MAX_CALLS 3

int callCount = 0;
int callDepth = 0;

void handler_siginfo(int signum, siginfo_t *info, void *context) {
    printf("siginfo handler\n");
    printf("- received signal: %d\n", info->si_signo);
    printf("- status: %d\n", info->si_status);
    printf("- sender PID: %d\n", info->si_pid);
    printf("- sender UID: %d\n", info->si_uid);
    printf("- utime: %ld\n", info->si_utime);
}

void handler_resethand(int signum, siginfo_t *info, void *context){
    printf("resethand handler\n");
}

void handler_nodefer(int signum, siginfo_t *info, void *context){
    int cur_id = callCount;
    printf("nodefer handler\n");
    printf("nodefer - start id: %d, depth: %d\n", cur_id, callDepth);
    callCount++;
    callDepth++;
    if(callCount < MAX_CALLS){
        raise(SIGUSR1);
    }
    callDepth--;
    printf("nodefer - end id: %d, depth: %d\n",cur_id, callDepth);
}

void test_siginfo() {
    printf("\nSA_SIGINFO TEST\n");
    struct sigaction action;
    action.sa_sigaction = handler_siginfo;
    action.sa_flags = SA_SIGINFO;
    sigemptyset(&action.sa_mask);
    sigaction(SIGUSR1, &action, NULL);
    raise(SIGUSR1);
}

void test_nodefer() {
    callCount = 0;
    printf("\nSA_NODEFER TEST\n");
    struct sigaction action;
    action.sa_sigaction = handler_nodefer;
    action.sa_flags = SA_NODEFER;
    sigemptyset(&action.sa_mask);
    sigaction(SIGUSR1, &action, NULL);
    raise(SIGUSR1);
}

void test_resethand() {
    callCount = 0;
    printf("\nSA_RESETHAND TEST\n");
    struct sigaction action;
    action.sa_sigaction = handler_resethand;
    action.sa_flags = SA_RESETHAND;
    sigemptyset(&action.sa_mask);
    sigaction(SIGUSR1, &action, NULL);
    raise(SIGUSR1);
    raise(SIGUSR1);
}

int main() {
    test_siginfo();
    test_nodefer();
    test_resethand();
    return 0;
}