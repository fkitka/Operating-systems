#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include "common.h"

key_t get_server_key(){
    key_t key = ftok(getenv("HOME"), SERVER_PROJ_ID);
    if (key == -1){
        fprintf(stderr, "Cannot generate a key\n");
        exit(1);
    }
    return key;
}

key_t get_client_key(){
    key_t key = ftok(getenv("HOME"), getpid());
    if (key == -1){
        fprintf(stderr, "Cannot generate a key\n");
        exit(1);
    }
    return key;
}

int create_queue(key_t key){
    int result = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
    if (result == -1){
        fprintf(stderr, "Cannot create a queue\n");
        exit(1);
    }
    return result;
}

int get_queue(key_t key){
    int result = msgget(key, 0);
    if (result == -1){
        fprintf(stderr, "Cannot get a queue\n");
        exit(1);
    }
    return result;
}

void receive_message(int msqid, message_t* message){
    if (msgrcv(msqid, message, MAX_MSG_LEN, (-7), 0) == -1){
        fprintf(stderr, "Error while receiving a message\n");
        exit(1);
    }
    // printf("message received id %d msqid %d\n", message->client_id, msqid);
}

void receive_message_flagged(int msqid, message_t* message, int msgflg){
    if (msgrcv(msqid, message, MAX_MSG_LEN, (-7), msgflg) == -1){
        fprintf(stderr, "Error while receiving a message flagged\n");
        exit(1);
    }
    // printf("message received id %d msqid %d\n", message->client_id, msqid);
}

void send_message(int msqid, message_t* message){
    // printf("message send id %d msqid %d\n", message->client_id, msqid);
    if (msgsnd(msqid, message, MAX_MSG_LEN, 0) == -1){
        fprintf(stderr, "Cannot send a message\n");
        exit(1);
    }
}
void delete_queue(int msqid){
    if(msgctl(msqid, IPC_RMID, NULL) == -1){
        fprintf(stderr, "Cannot delete a queue: %d\n", msqid);
        exit(1);
    }
}