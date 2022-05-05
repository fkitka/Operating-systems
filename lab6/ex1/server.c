#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include "common.h"
#include <string.h>
#include <time.h>

#define MAX_CLIENTS 10
#define CLIENT_MSQID 0
#define CLIENT_PID 1
#define FILENAME "logs.txt"

int server_msqid;
int clients[MAX_CLIENTS][2]; // client_msqid, client_pid; [i] - client_id
int client_is_set[MAX_CLIENTS];

int all_shutdown(int clients_array[]);
int is_empty(int msqid);
void stop_server();
void init_clients();
void handle_sigint();
void init_signal();
void init();
void handle_stop(message_t* message);
void handle_list(message_t *message);
void handle_2all(message_t *message);
void handle_2one(message_t *message);
void handle_init(message_t *message);
void handle_message(message_t message);
void save_message(message_t message);
int get_free_id();

int all_shutdown(int clients_array[]){
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(clients_array[i] != 0){
            return 0;
        }
    }
    return 1;
}

int is_empty(int msqid) {
    struct msqid_ds msqid_buffer;
    msgctl(msqid, IPC_STAT, &msqid_buffer);
    if(msqid_buffer.msg_qnum != 0) return 0;
    else return 1;
}

void stop_server(){
    printf("Server closing\n");
    message_t stop_message;
    stop_message.type = STOP;
    for (int i = 0; i < MAX_CLIENTS; i++){
        if(client_is_set[i] == 1){
            printf("Stop sent to client id: %d\n", i);
            stop_message.client_id = i;
            send_message(clients[i][CLIENT_MSQID], &stop_message);
        }
    }

    message_t client_message;
    while (!all_shutdown(client_is_set)){
        receive_message(server_msqid, &client_message);
        if(client_message.type == STOP){
            client_is_set[client_message.client_id] = 0;
            printf("Stop received from client %d\n", client_message.client_id);
        }
        else{
            fprintf(stderr, "Waiting for STOP\n");
        }
    }
  
    delete_queue(server_msqid);
    printf("Server queue deleted\n");
    printf("Server closed successfully\n");
}

void init_clients() {
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        client_is_set[i] = 0;
        clients[i][CLIENT_MSQID]= -1;
        clients[i][CLIENT_PID] = -1;
    }
    printf("Clients initiated\n");
}

void handle_sigint(){
    printf("Server exit signal sent\n");
    exit(0);
}

void init_signal() {
    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = handle_sigint;
    sigaction(SIGINT, &action, NULL);
}

void init() {
    key_t key = get_server_key();
    server_msqid = create_queue(key);
    init_clients();
    init_signal();
    printf("Server initiaion - success\n");
}

void handle_stop(message_t* message) {
    printf("Server: STOP received\n");
    int client_id = message->client_id;
    client_is_set[client_id] = 0;
}

void handle_list(message_t *message) {
    printf("Server: LIST received\n");
    char body[MAX_MSG_LEN];
    body[0] = '\0';
    char buff[32];
    
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if(client_is_set[i] == 1){
            printf("Client_id: %d, Client_msqid: %d\n", i, clients[i][CLIENT_MSQID]);
            sprintf(buff, "Clients_id %d\n", i);
            strcat(body, buff);
        }
    }
    message_t list_message;
    list_message.type = LIST;
    snprintf(list_message.text, MAX_MSG_LEN, "%s", body);
    send_message(clients[message->client_id][CLIENT_MSQID], &list_message);
    printf("\n");
}

void handle_2all(message_t *message) {
    printf("Server: 2ALL received\n");
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if(client_is_set[i] == 1 && i != message->client_id){
            send_message(clients[i][CLIENT_MSQID], message);
            printf("Message sent to client_id:%d\n", i);
        }
    }
    printf("\n");
}

void handle_2one(message_t *message) {
    printf("Server: 2ONE received\n");
    if(client_is_set[message->receiver_id] == -1){
        fprintf(stderr, "Client not available\n");
        return;
    }
    send_message(clients[message->receiver_id][CLIENT_MSQID], message);
    printf("Message sent to client_id: %d\n", message->receiver_id);
    printf("\n");
}

int get_free_id() {
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if(client_is_set[i] == 0){
            return i;
        }
    }
    return -1;
}

void handle_init(message_t *message) {
    printf("Server: INIT received\n");
    key_t key = (key_t) strtol(message->text, NULL, 10);
    int client_pid = message->client_pid;
    int client_msqid = get_queue(key);
    int client_id = get_free_id();
    printf("New client id - %d\n", client_id);
    if(client_id == -1){
        printf("Cannot assign id to client, too many clients\n");
        message_t error_message;
        sprintf(error_message.text, "%s", "Too many clients, cannot join the server\n");
        error_message.type = ERROR;
        error_message.client_pid = client_pid;
        error_message.receiver_id = -1;
        error_message.client_id = -1;
        send_message(client_msqid, &error_message);
        return;
    }

    clients[client_id][CLIENT_MSQID] = client_msqid;
    clients[client_id][CLIENT_PID] = client_pid;
    client_is_set[client_id] = 1;

    message_t init_message;
    init_message.type = INIT;
    sprintf(init_message.text, "%s", "Successfully joined the server\n");
    init_message.receiver_id = client_id;
    init_message.client_id = client_id;
    send_message(client_msqid, &init_message);
    printf("\n");
}

void save_message(message_t message){
    FILE *logs_fp = fopen(FILENAME, "a");
    if(logs_fp == NULL){
        fprintf(stderr, "Cannot open the file\n");
        exit(1);
    }
    struct tm *local_time = localtime(&message.timestamp);
    int res = fprintf(logs_fp, "%d-%02d-%02d %02d:%02d:%02d\nClient id: %d\nMessage type: %ld\nMessage text:\n'%s'\n\n",
                local_time->tm_year + 1900,
                local_time->tm_mon + 1,
                local_time->tm_mday,
                local_time->tm_hour,
                local_time->tm_min,
                local_time->tm_sec,
                message.client_id,
                message.type,
                message.text
    );
    if (res == -1){
        fprintf(stderr, "Cannot write to the file\n");
    }
    fclose(logs_fp);
}

void handle_message(message_t message) {
    switch(message.type){
        case STOP:
            handle_stop(&message);
            break;
        case LIST:
            handle_list(&message);
            break;
        case TO_ALL:
            handle_2all(&message);
            break;
        case TO_ONE:
            handle_2one(&message);
            break;
        case INIT:
            handle_init(&message);
            break;
        default:
            fprintf(stderr, "Wrong message type\n");
            exit(1);
    }
    save_message(message);
}

int main(){
    printf("[ SERVER ]\n");
    atexit(stop_server);
    init();
    while(1){
        message_t message;
        receive_message(server_msqid, &message);
        handle_message(message);
    }
}
