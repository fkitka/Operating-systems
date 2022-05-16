#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <math.h>

#define MAX_ROW_LEN 256

int threadsCount;
int W, H;

int **outputImage;

int **inputImage;

void loadInput(char *filename);

void initOutput();

double * getThreadInfo(pthread_t *threads);

void saveResults(char *filename);

void runNumbers(pthread_t *threads);

void runBlock(pthread_t *threads);

void *numbersRoutine(void *arg);

void *blockRoutine(void *arg);

void saveThreadLogs(struct timeval start, struct timeval stop, double *threadTimes, char *variant);

int main(int argc, char* argv[]) {
    if (argc != 5){
        fprintf(stderr, "Wrong number of arguments\n");
        exit(1);
    }

    threadsCount = (int)strtol(argv[1], NULL, 10);
    char *variant = argv[2];
    char *inputFilename = argv[3];
    char *outputFilename = argv[4];

    pthread_t *threads = calloc(threadsCount, sizeof(pthread_t));

    loadInput(inputFilename);

    initOutput();

    struct timeval startTime, stopTime;

    gettimeofday(&startTime, NULL);

    if(strcmp(variant, "numbers") == 0){
        runNumbers(threads);
    }
    else if(strcmp(variant, "block") == 0){
        runBlock(threads);
    }
    else{
        fprintf(stderr, "Invalid command\n");
        exit(1);
    }

    double *threadTimes = getThreadInfo(threads);

    gettimeofday(&stopTime, NULL);

    saveThreadLogs(startTime, stopTime, threadTimes, variant);

    saveResults(outputFilename);

    free(threads);
    return 0;
}

void saveResults(char *filename) {
    FILE *fp = fopen(filename, "w");
    printf("> Opening output file\n");
    if(fp == NULL){
        fprintf(stderr, "Cannot open the file\n");
        exit(1);
    }
    printf("> Writing output\n");
    fprintf(fp, "P2\n");
    fprintf(fp, "%d %d\n", W, H);
    fprintf(fp, "%d\n", 255);
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            fprintf(fp, "%d ", outputImage[i][j]);
        }
        fprintf(fp, "\n");
    }
    printf("> Results written.\n");
    printf("> Closing file\n");
    fclose(fp);

}

double * getThreadInfo(pthread_t *threads) {
    double *thread_times = calloc(threadsCount, sizeof(double));
    for (int i = 0; i < threadsCount; ++i) {
        double *t;
        if(pthread_join(threads[i], (void **) &t) != 0){
            fprintf(stderr, "Error while thread join\n");
            exit(1);
        }
        printf(">> Thread info: thread: %d, time: %lf\n", i, *t);
        thread_times[i] = *t;
    }
    printf("-----------------------------------------\n");
    return thread_times;
}

void saveThreadLogs(struct timeval start, struct timeval stop, double *threadTimes, char *variant) {
    printf("> Opening logs file\n");
    printf("> Writing logs\n");
    FILE *fp = fopen("Times.txt", "a");
    fprintf(fp,"----------------------------\n");
    fprintf(fp, "Number of threads: %d\n", threadsCount);
    fprintf(fp, "Size of image: W: %d x H: %d\n", W, H);
    fprintf(fp, "Variant: %s\n", variant);
    for (int i = 0; i < threadsCount; ++i) {
        fprintf(fp, "thread: %d, time: %lf\n", i, threadTimes[i]);
    }
    double *t = calloc(1, sizeof(double));
    *t = (double)((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
    printf(">> total time: %lf\n", *t);
    fprintf(fp, "total time: %lf\n", *t);
    fprintf(fp,"----------------------------\n");
    printf("> Logs written.\n");
    printf("> Closing file\n");
}


void initOutput() {
    printf("> Initializing output matrix\n");
    outputImage = calloc(H, sizeof(int*));
    for (int i = 0; i < H; ++i) {
        outputImage[i] = calloc(W, sizeof(int));
    }
}

void loadInput(char *filename) {
    int M;
    FILE* fp = fopen(filename, "r");
    if(fp == NULL){
        fprintf(stderr, "Cannot open the fp\n");
        exit(1);
    }
    printf("> Opening input file\n");
    printf("> Loading input\n");
    char* buffer = calloc(MAX_ROW_LEN, sizeof(char));
    fgets(buffer, MAX_ROW_LEN, fp);
    fgets(buffer, MAX_ROW_LEN, fp);
    sscanf(buffer, "%d %d\n", &W, &H);

    fgets(buffer, MAX_ROW_LEN, fp);
    sscanf(buffer, "%d\n", &M);

    inputImage = calloc(H, sizeof(int *));
    for (int i = 0; i < H; ++i) {
        inputImage[i] = calloc(W, sizeof(int));
        for (int j = 0; j < W; ++j) {
            fscanf(fp, "%d", &inputImage[i][j]);
        }
    }

    printf("> Input loaded.\n");
    printf("> Closing file\n");
    fclose(fp);
}

void runNumbers(pthread_t *threads) {
    printf("> Run numbers variant\n");
    int *threadsId = calloc(threadsCount, sizeof(int));
    printf("> Creating threads\n");
    printf("-----------------------------------------\n");
    for (int i = 0; i < threadsCount; ++i) {
        threadsId[i] = i;
        pthread_create(&threads[i], NULL, &numbersRoutine, &threadsId[i]);
    }
}

void *numbersRoutine(void *arg) {
    struct timeval stopTime, startTime;

    gettimeofday(&startTime, NULL);

    int id = *(int *) arg;
    printf(">>> Thread %d working...\n", id);
    int counter = 0;
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            if(counter % threadsCount == id){
                outputImage[i][j] = 255 - inputImage[i][j];
            }
            counter++;
        }
    }

    gettimeofday(&stopTime, NULL);

    double *t = calloc(1, sizeof(double));
    *t = (double)((stopTime.tv_sec - startTime.tv_sec) * 1000000 + stopTime.tv_usec - startTime.tv_usec);
    printf(">>> Thread %d work done.\n", id);
    pthread_exit(t);
}

void runBlock(pthread_t *threads) {
    printf("> Run block variant\n");
    int *threads_id = calloc(threadsCount, sizeof(int));
    printf("> Creating threads\n");
    printf("-----------------------------------------\n");
    for (int i = 0; i < threadsCount; ++i) {
        threads_id[i] = i;
        pthread_create(&threads[i], NULL, &blockRoutine, &threads_id[i]);
    }
}

void *blockRoutine(void *arg) {
    struct timeval stopTime, startTime;

    gettimeofday(&startTime, NULL);
    int id = *(int *) arg;
    int from =(int)(id * ceil((double)W / threadsCount));
    int to = (int)((id+1) * ceil((double)W / threadsCount)) - 1;
    printf("from %d, to %d\n", from, to);
    printf(">>> Thread %d working...\n", id);
    for (int i = 0; i < H; ++i) {
        for (int j = from; j <= to ; ++j) {
            outputImage[i][j] = 255 - inputImage[i][j];
        }
    }

    gettimeofday(&stopTime, NULL);

    double *t = calloc(1, sizeof(double));
    *t = (double)((stopTime.tv_sec - startTime.tv_sec) * 1000000 + stopTime.tv_usec - startTime.tv_usec);
    printf(">>> Thread %d work done.\n", id);
    pthread_exit(t);
}
