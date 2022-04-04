#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define SIGNAL SIGUSR1

void check_is_pending(){
    sigset_t pending;
    sigpending(&pending);
    if(sigismember(&pending, SIGNAL)){
        printf("Exec - Signal is pending\n");
    }
    else printf("Exec - Signal is not pending\n");
}

int main(){
    check_is_pending();
    raise(SIGNAL);
    return 0;
}
