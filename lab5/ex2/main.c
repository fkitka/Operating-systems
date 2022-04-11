#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_COMMAND_SIZE 256
#define MAX_LINE_SIZE 256

void show_sorted_emails(char *arg);

void send_email(char *email, char *subject, char *content);

int parse_arg(char *arg);

int main(int argc, char* argv[]) {
    if(argc != 2 && argc != 4){
        printf("To sort emails use ./main nadawca or ./main data\n");
        printf("To send an email use ./main <adresEmail> <tytul> <tresc>\n");
        fprintf(stderr, "Wrong number of arguments\n");
        exit(1);
    }
    if(argc == 2){
        // show sorted email list alphabetically <nadawca> or chronologically <data>
        show_sorted_emails(argv[1]);
    }
    else{
        // send an email to <adresEmail> with <tytul> and <tresc>
        char* email = argv[1];
        char* subject = argv[2];
        char* content = argv[3];
        send_email(email, subject, content);
    }

    return 0;
}

void send_email(char *email, char *subject, char *content) {
    FILE *fp;
    char command[MAX_COMMAND_SIZE];
    sprintf(command, "mail -s %s %s", subject, email);
    fp = popen(command, "w");
    if(fp == NULL){
        fprintf(stderr, "Error with popen\n");
        exit(1);
    }
    fputs(content, fp);
    pclose(fp);
}

void show_sorted_emails(char *arg) {
    FILE *fp;
    int mode = parse_arg(arg);
    char* command;
    switch(mode){
        case 0:
            command = "echo | mail -f | tail +2 | head -n -2 | sort -k 2";
            break;
        case 1:
            command = "echo | mail -f | tail +2 | head -n -2 | sort -r";
            break;
        default:
            fprintf(stderr, "Invalid argument\n");
            exit(1);
    }
    fp = popen(command, "r");
    if(fp == NULL){
        fprintf(stderr,"Error with popen\n");
        exit(1);
    }

    //print output
    char buffor[MAX_LINE_SIZE];
    while(fgets(buffor, MAX_LINE_SIZE, fp)!= NULL){
        printf("%s\n", buffor);
    }
    pclose(fp);
}

int parse_arg(char *arg) {
    if(strcmp(arg, "nadawca") == 0){
        return 0;
    }
    else if(strcmp(arg, "data") == 0){
        return 1;
    }
    return -1;
}
