#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define READ 0
#define WRITE 1

typedef struct Command {
    char **innerCommand;
    int argumentsCount;
} Command;

typedef struct Element {
    char *name;
    Command *commands;
    int commandsCount;
} Element;

void parseFile(FILE *fp);

void parseLine(char *line, Element *element);

Command getCommandsFromElement(char *line);

Command **getCommandsToExecute(char *line, Element *elements, int count, int *commandsCount);

void executeCommands(Command **commands, int count);

void printCommands(Command **commands, int commandsCount);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Provide a path to the file with commands.\n");
        fprintf(stderr, "Wrong number of arguments\n");
        exit(1);
    }
    char *path = argv[1];
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr, "Cannot open the file\n");
        exit(1);
    }
    parseFile(fp);
    fclose(fp);
    return 0;
}

void parseFile(FILE *fp) {
    char *line = calloc(256, sizeof(char));
    int elementsCount = 0;
    Element *elements = calloc(1, sizeof(Element));
    while (fgets(line, 256 * sizeof(char), fp)) {
        if (strchr(line, '=') != NULL) {
            parseLine(line, &elements[elementsCount]);
            elementsCount++;
            elements = realloc(elements, (elementsCount + 1) * sizeof(Element));

        } else {
            int commandsCount = 0;
            Command **commands = getCommandsToExecute(line, elements, elementsCount, &commandsCount);
            if (*commands != NULL) {
                printf("Executing -> ");
                printCommands(commands, commandsCount);
                executeCommands(commands, commandsCount);
                printf("----------------------------\n");
            }

        }

    }
    free(line);
}

void parseLine(char *line, Element *element) {
    char *x = strtok(line, " =");
    element->name = calloc(strlen(x), sizeof(char));
    strcpy(element->name, x);
    element->commandsCount = 0;
    strtok(NULL, " ");
    char **singleCommands = calloc(1, sizeof(char *));
    int i = 0;
    while ((x = strtok(NULL, "|")) != NULL) {
        singleCommands[i] = x;
        element->commandsCount++;
        element->commands = realloc(element->commands, element->commandsCount * sizeof(Command));
        singleCommands = realloc(singleCommands, element->commandsCount * sizeof(char *));
        i++;
    }
    for (i = 0; i < element->commandsCount; i++) {
        element->commands[i] = getCommandsFromElement(singleCommands[i]);
    }

}

Command getCommandsFromElement(char *line) {
    Command command;
    command.innerCommand = calloc(2, sizeof(char *));
    int argumentsCount = 0;
    char *x = strtok(line, " \t\n");
    command.innerCommand[argumentsCount] = calloc(1, sizeof(x));
    strcpy(command.innerCommand[argumentsCount], x);
    while ((x = strtok(NULL, " \t\n")) != NULL) {
        argumentsCount++;
        command.innerCommand[argumentsCount] = calloc(1, sizeof(x));
        strcpy(command.innerCommand[argumentsCount], x);
        command.innerCommand = realloc(command.innerCommand, (argumentsCount + 1) * sizeof(char));
    }
    command.argumentsCount = argumentsCount + 1;
    return command;
}


Command **getCommandsToExecute(char *line, Element *elements, int count, int *commandsCount) {
    char *x = strtok(line, " | \n\t");
    *commandsCount = 0;
    Command **resultCommands = calloc(1, sizeof(Command *));
    int id = 0;
    while (x != NULL) {
        for (int i = 0; i < count; ++i) {
            if (strcmp(x, elements[i].name) == 0) {
                for (int j = 0; j < elements[i].commandsCount; ++j) {
                    *commandsCount = *commandsCount + 1;
                    resultCommands[id] = &elements[i].commands[j];
                    id++;
                    resultCommands = realloc(resultCommands, (id + 1) * sizeof(Command *));
                }
            }
        }
        x = strtok(NULL, "| \n\t");
    }
    return resultCommands;
}

void executeCommands(Command **commands, int count) {
    int **pipes = calloc(count, sizeof(int *));
    for (int i = 0; i < count; ++i) {
        pipes[i] = calloc(2, sizeof(int));
        if (pipe(pipes[i]) < 0) {
            fprintf(stderr, "Cannot make the pipe\n");
            exit(1);
        }
    }

    for (int i = 0; i < count; ++i) {
        int pid = fork();
        if (pid == 0) {
            if (i > 0) {
                close(pipes[i - 1][WRITE]);
                dup2(pipes[i - 1][READ], STDIN_FILENO);
            }
            if (i + 1 < count) {
                close(pipes[i][READ]);
                dup2(pipes[i][WRITE], STDOUT_FILENO);
            }
            for (int j = 0; j < count; ++j) {
                close(pipes[j][READ]);
                close(pipes[j][WRITE]);
            }
            execvp(commands[i]->innerCommand[0], commands[i]->innerCommand);
            exit(0);
        }
    }
    for (int i = 0; i < count; ++i) {
        close(pipes[i][READ]);
        close(pipes[i][WRITE]);
    }
    for (int i = 0; i < count; ++i) {
        wait(0);
    }
    for (int i = 0; i < count; i++) {
        free(pipes[i]);
    }
    free(pipes);
}

void printCommands(Command **commands, int commandsCount) {
    for (int i = 0; i < commandsCount; ++i) {
        for (int j = 0; j < commands[i]->argumentsCount; ++j) {
            printf("%s ", commands[i]->innerCommand[j]);
        }
        if (i < commandsCount - 1)
            printf("| ");
    }
    printf("\n");
}




