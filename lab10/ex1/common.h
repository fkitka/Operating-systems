#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <poll.h>


#define MAX_CLIENTS 12
#define MAX_MSG_LEN 256

void error(char* msg){
    perror(msg);
    exit(1);
}

typedef struct Client {
    char name[32];
    int socket;
    int available;
    int opponent;
} client_t;

char* recvMsg(int fd){
    char *buf = calloc(MAX_MSG_LEN, sizeof(char));
    if(read(fd, buf, MAX_MSG_LEN) == -1){
        error("Cannot receive message");
    }
    return buf;
}

void sendMsg(int fd, char* buf){
    if(write(fd, buf, MAX_MSG_LEN) == -1){
        error("Cannot send message");
    }
}

