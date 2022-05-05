#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "common.h"

int server_msqid;
int client_msqid;
int client_id;

pid_t catcher_pid;

void stop_client();

void init();

void send_list();

void send_stop();

void send_2all(const char *text);

void send_2one(const char *text, char* receiver);

void parse_command(char *input);

void input_commands();

int is_empty(int msqid);

void handle_stop();

void handle_2all(message_t *message);

void handle_2one(message_t *message);

void catcher();

void handle_sigint();

void handle_list();

void init_sigint();

int main(){
    printf("[ CLIENT ]\n");
    atexit(stop_client);
    init();
    if((catcher_pid = fork()) == -1){
        fprintf(stderr, "Cannot create a child process\n");
        exit(1);
    }
    if(catcher_pid == 0){
        while(1){
            catcher();
        }
    }
    else{
        while(1){
            input_commands();
        }
    }
    wait(NULL);
    return 0;
}


void init_sigint() {
    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = handle_sigint;
    sigaction(SIGINT, &action, NULL);
}

void stop_client() {
    delete_queue(client_msqid);
    kill(catcher_pid, SIGKILL);
    printf("Client queue deleted\n");
    printf("Client closed successfully\n");
}

void init() {
    init_sigint();
    key_t server_key = get_server_key();
    server_msqid = get_queue(server_key);

    key_t client_key = get_client_key();
    client_msqid = create_queue(client_key);

    message_t init_message;
    init_message.type = INIT;
    snprintf(init_message.text, MAX_MSG_LEN-1, "%d", client_key);
    init_message.client_pid = getpid();
    init_message.timestamp = time(NULL);
    send_message(server_msqid, &init_message);

    message_t message_received;
    receive_message(client_msqid, &message_received);
    client_id = message_received.client_id;
    printf("%s", message_received.text);
    printf("Client id - %d\n", client_id);
    if(message_received.type == ERROR){
        exit(1);
    }
}

void send_list() {
    printf("Client sent LIST\n");
    message_t message;
    message.type = LIST;
    message.client_id = client_id;
    message.timestamp = time(NULL);
    sprintf(message.text, "List all active clients"); // just for logs
    send_message(server_msqid, &message);
}

void send_stop() {
    printf("Client sent STOP\n");
    message_t message;
    message.type = STOP;
    message.client_id = client_id;
    message.timestamp = time(NULL);
    sprintf(message.text, "Stop client"); // just for logs
    send_message(server_msqid, &message);
    exit(0);
}

void send_2all(const char *text) {
    printf("Client sent 2ALL\n");
    message_t message;
    message.client_id = client_id;
    message.timestamp = time(NULL);
    sprintf(message.text,"%s",text);
    message.type = TO_ALL;
    send_message(server_msqid, &message);
}

void send_2one(const char *text, char* receiver) {
    printf("Client sent 2ONE\n");
    message_t message;
    message.type = TO_ONE;
    message.client_id = client_id;
    message.client_pid = getpid();
    message.timestamp = time(NULL);
    strcpy(message.text,text);
    message.receiver_id = (int)strtol(receiver, NULL, 10);
    send_message(server_msqid, &message);
}

void parse_command(char *input) {
    if (strcmp("LIST\n", input)==0){
        send_list();
    }
    else if(strcmp("STOP\n", input) == 0){
        send_stop();
    }
    else if(strcmp("2ALL\n", input) == 0){
        printf("Message> ");
        char* text = calloc(MAX_MSG_LEN, sizeof(char));
        fgets(text, MAX_MSG_LEN, stdin);
        printf("\n");
        send_2all(text);
    }
    else if(strcmp("2ONE\n", input) == 0){
        printf("Message> ");
        char* text = calloc(MAX_MSG_LEN, sizeof(char));
        fgets(text, MAX_MSG_LEN, stdin);
        printf("Receiver_id> ");
        char* receiver = calloc(MAX_MSG_LEN, sizeof(char));
        fgets(receiver, MAX_MSG_LEN, stdin);
        printf("\n");
        send_2one(text, receiver);

    }
    else{
        printf("Unknown command: %s\n", input);
    }
}

void input_commands() {
    char* input = calloc(MAX_MSG_LEN, sizeof(char));
    printf("> ");
    fgets(input, MAX_MSG_LEN, stdin);
    parse_command(input);
}

int is_empty(int msqid) {
    struct msqid_ds msqid_buffer;
    msgctl(msqid, IPC_STAT, &msqid_buffer);
    if(msqid_buffer.msg_qnum != 0) return 0;
    else return 1;
}

void handle_stop() {
    printf("Client received STOP\n");
    message_t message;
    message.type = STOP;
    message.client_id = client_id;
    message.timestamp = time(NULL);
    send_message(server_msqid, &message);
    printf("Client sent STOP to the server\n");
    exit(0);
}

void handle_2all(message_t *message) {
    printf("Client received 2ALL\n");
    time_t send_time = message->timestamp;
    struct tm *local_time = localtime(&send_time);
    if (!local_time) {
        fprintf(stderr, "Unable to get a local time.\n");
        exit(1);
    }

    printf("%d-%02d-%02d %02d:%02d:%02d\nClient id: %d\nMessage type: %ld\nMessage text:\n%s \n",
           local_time->tm_year + 1900, local_time->tm_mon + 1, local_time->tm_mday,
           local_time->tm_hour, local_time->tm_min, local_time->tm_sec,
           message->client_id, message->type, message->text);
}

void handle_2one(message_t *message) {
    printf("Client received 2ONE\n");
    time_t send_time = message->timestamp;
    struct tm *local_time = localtime(&send_time);
    if (!local_time) {
        fprintf(stderr, "Unable to get a local time.\n");
        exit(1);
    }
    printf("%d-%02d-%02d %02d:%02d:%02d\nClient id: %d\nMessage type: %ld\nMessage text:\n%s \n",
           local_time->tm_year + 1900, local_time->tm_mon + 1, local_time->tm_mday,
           local_time->tm_hour, local_time->tm_min, local_time->tm_sec,
           message->client_id, message->type, message->text);
}

void catcher() {
    while(!is_empty(client_msqid)){
        message_t message;
        receive_message_flagged(client_msqid, &message, IPC_NOWAIT);
        switch(message.type){
            case STOP:
                handle_stop();
                exit(0);
            case TO_ALL:
                handle_2all(&message);
                break;
            case TO_ONE:
                handle_2one(&message);
                break;
            case LIST:
                handle_list(&message);
            default:
                break;
        }
    }
}

void handle_sigint() {
    send_stop();
    printf("Client is closing\n");
}

void handle_list(message_t *message){
    printf("%s", message->text);
}