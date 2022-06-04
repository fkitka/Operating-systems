#include <stdio.h>
#include <threads.h>
#include <sys/types.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define REINDEERS_COUNT 9
#define ELVES_COUNT 10

int elvesWaiting = 0;

int areReindeersWaiting = 0;

int reindeersBack = 0;

int deliveriesCount = 0;

int elvesQueue[ELVES_COUNT];

pthread_mutex_t elvesWaitingMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t elvesMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t santaMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t reindeerWaitingMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t reindeerMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t santaCond = PTHREAD_COND_INITIALIZER;

pthread_cond_t elvesWaitingCond = PTHREAD_COND_INITIALIZER;

pthread_cond_t reindeerWaitingCond = PTHREAD_COND_INITIALIZER;

void *santa_routine(void *arg);

void *reindeer_routine(void *arg);

void *elf_routine(void *arg);

int rand_time(int min, int max) {
    return (rand() % (max - min + 1)) + min;
}

int main() {
    srand(time(NULL));
    pthread_t santa;
    pthread_create(&santa, NULL, &santa_routine, NULL);

    pthread_t *reindeers = calloc(REINDEERS_COUNT, sizeof(pthread_t));
    int *reindeers_id = calloc(REINDEERS_COUNT, sizeof(int));
    for (int i = 0; i < REINDEERS_COUNT; ++i) {
        reindeers_id[i] = i;
        pthread_create(&reindeers[i], NULL, &reindeer_routine, &reindeers_id[i]);
    }

    pthread_t *elves = calloc(ELVES_COUNT, sizeof(pthread_t));
    int *elves_id = calloc(ELVES_COUNT, sizeof(int));
    for (int i = 0; i < ELVES_COUNT; ++i) {
        elves_id[i] = i;
        pthread_create(&elves[i], NULL, &elf_routine, &elves_id[i]);
    }

    pthread_join(santa, NULL);

    for (int i = 0; i < REINDEERS_COUNT; ++i) {
        pthread_join(reindeers[i], NULL);
    }

    for (int i = 0; i < ELVES_COUNT; ++i) {
        pthread_join(elves[i], NULL);
    }

    pthread_mutex_destroy(&elvesWaitingMutex);

    pthread_mutex_destroy(&elvesMutex);

    pthread_mutex_destroy(&santaMutex);

    pthread_mutex_destroy(&reindeerWaitingMutex);

    pthread_mutex_destroy(&reindeerMutex);

    pthread_cond_destroy(&santaCond);

    pthread_cond_destroy(&elvesWaitingCond);

    pthread_cond_destroy(&reindeerWaitingCond);


    return 0;
}

void *elf_routine(void *arg) {
    int id = *(int *) arg;
    while (1) {
        sleep(rand_time(2, 5));

        pthread_mutex_lock(&elvesWaitingMutex);
        while (elvesWaiting >= 3) {
            printf("Elf: czeka na powrót elfów, %d\n", id);
            pthread_cond_wait(&elvesWaitingCond, &elvesWaitingMutex);
        }
        pthread_mutex_unlock(&elvesWaitingMutex);

        pthread_mutex_lock(&elvesMutex);
        if (elvesWaiting < 3) {
            elvesQueue[elvesWaiting] = id;
            elvesWaiting++;

            printf("Elf: czeka %d elfów na Mikołaja, %d\n", elvesWaiting, id);

            if (elvesWaiting == 3) {
                pthread_mutex_lock(&santaMutex);
                pthread_mutex_unlock(&santaMutex);
                printf("Elf: wybudzam Mikołaja, %d\n", id);
                pthread_cond_broadcast(&santaCond);
            }
        }
        pthread_mutex_unlock(&elvesMutex);
    }
}

void *reindeer_routine(void *arg) {
    int id = *(int *) arg;
    while (1) {
        pthread_mutex_lock(&reindeerWaitingMutex);
        while (areReindeersWaiting) {
            pthread_cond_wait(&reindeerWaitingCond, &reindeerWaitingMutex);
        }
        pthread_mutex_unlock(&reindeerWaitingMutex);

        sleep(rand_time(5, 10));

        pthread_mutex_lock(&reindeerMutex);
        reindeersBack++;
        printf("Renifer: czeka %d reniferow na Mikołaja, %d\n", reindeersBack, id);
        areReindeersWaiting = 1;

        if (reindeersBack == REINDEERS_COUNT) {
            pthread_mutex_lock(&santaMutex);
            pthread_mutex_unlock(&santaMutex);
            printf("Renifer: wybudzam Mikołaja, %d\n", id);
            pthread_cond_broadcast(&santaCond);
        }
        pthread_mutex_unlock(&reindeerMutex);
    }
}

void *santa_routine(void *arg) {
    while (1) {
        pthread_mutex_lock(&santaMutex);
        while (elvesWaiting < 3 && reindeersBack < 9) {
            pthread_cond_wait(&santaCond, &santaMutex);
        }
        pthread_mutex_unlock(&santaMutex);

        printf("Mikołaj: budzę się\n");

        pthread_mutex_lock(&reindeerMutex);
        if (reindeersBack == REINDEERS_COUNT) {
            printf("Mikołaj: dostarczam zabawki\n");
            sleep(rand_time(2, 4));
            deliveriesCount++;

            reindeersBack = 0;

            pthread_mutex_lock(&reindeerWaitingMutex);
            areReindeersWaiting = 0;
            pthread_mutex_unlock(&reindeerWaitingMutex);
            pthread_cond_broadcast(&reindeerWaitingCond);
        }
        pthread_mutex_unlock(&reindeerMutex);

        if (deliveriesCount == 3) {
            exit(0);
        }

        pthread_mutex_lock(&elvesMutex);
        if (elvesWaiting == 3) {
            printf("Mikołaj: rozwiązuje problemy elfów %d %d %d\n", elvesQueue[0], elvesQueue[1], elvesQueue[2]);
            sleep((unsigned int) rand_time(1, 2));

            pthread_mutex_lock(&elvesWaitingMutex);
            elvesWaiting = 0;
            pthread_mutex_unlock(&elvesWaitingMutex);
            pthread_cond_broadcast(&elvesWaitingCond);
        }
        pthread_mutex_unlock(&elvesMutex);

        printf("Mikołaj: zasypiam\n");

    }
}
