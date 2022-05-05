#include <sys/msg.h>

#define MAX_MSG_LEN 4096
#define STOP 1
#define LIST 2
#define TO_ALL 3
#define TO_ONE 4
#define INIT 5
#define ERROR 6
#define SERVER_PROJ_ID 'F'

struct message_t {
    long type;
    int client_id;
    int receiver_id;
    int client_pid;
    time_t timestamp;
    char text[MAX_MSG_LEN];
};
typedef struct message_t message_t;

key_t get_server_key();
key_t get_client_key();
int create_queue(key_t key);
int get_queue(key_t key);
void receive_message(int msqid, message_t* message);
void receive_message_flagged(int msqid, message_t* message, int msgflg);
void send_message(int msqid, message_t* message);
void delete_queue(int msqid);