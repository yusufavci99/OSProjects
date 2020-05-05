//Macros
#define FILE_NAME_SIZE 32
#define MAX_LINE_SIZE 128
#define DONE "DONE"
#define MAX_K 10
#define MIN_K 1
#define MAX_B 10000
#define MIN_B 100

//Includes
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>

//Globals
int K, B;
int splitSize, vectorSize;
char *matrixFileName;
char *vectorFileName;
char *resultFileName;
int* vectorValues;
int* resultVector;
char** splitFileNames;
sem_t **sem_mutex;
sem_t **sem_empty;
struct Buffer** buffers;

//Function Definitions
void setValues(int argc, char *argv[]);
int getLineCount(char *fileName);
char** generateSplitFiles(char* matrixFile, int splitSize);
void *mapper(void *param);
void *reducer(void *param);
struct Buffer** getBuffers();
char* pop(struct Buffer *buffer);
void push(struct Buffer *buffer, char* line);

//Buffer node definition
struct Buffer {
    //Maximum line capacity of the buffer
    int capacity;
    //Current size of the buffer
    int size;
    int front;
    int end;
    char **lines;
};

//main
int main(int argc, char *argv[]) {

    //Sets 
    setValues(argc, argv);

    splitSize = getLineCount(matrixFileName) / K;

    //An array containing names of split files to open in mapper.
    splitFileNames = generateSplitFiles(matrixFileName, splitSize);

    vectorSize = getLineCount(vectorFileName);
    vectorValues = malloc(vectorSize * sizeof(int));
    resultVector = calloc(vectorSize, sizeof(int));

    //Read vector file and fill the vector array.
    FILE* vectorFile = fopen(vectorFileName, "r");

    int column, value;

    while(fscanf( vectorFile, "%d %d", &column, &value) != EOF) {
        if(column > vectorSize || column <= 0) {
            printf("%s\n", "Invalid Vector File!");
            exit(1);
        }
        vectorValues[column - 1] = value;
    }

    fclose(vectorFile);

    sem_mutex = malloc(K * sizeof(sem_t*));
    sem_empty = malloc(K * sizeof(sem_t*));
    for(int i = 0; i < K; i++) {

        sem_mutex[i] = malloc(sizeof(sem_t));
        sem_init(sem_mutex[i], 0, 1);
        if (sem_mutex[i] < 0) {
            perror("Cannot create semaphore\n");
            exit (1); 
        }

        sem_empty[i] = malloc(sizeof(sem_t));
        sem_init(sem_empty[i], 0, B);
        if (sem_empty[i] < 0) {
            perror("Cannot create semaphore\n");
            exit (1); 
        } 
    }

    buffers = getBuffers();

    pthread_t tid[K];
    pthread_attr_t attr[K];

    //Create as many mapper threads as split files
    for(int mapperCount = 0; mapperCount < K; mapperCount++) {
        //Get the default attributes.
        pthread_attr_init(&attr[mapperCount]);
        //Create the thread
        int* count = malloc(sizeof(int));
        *count = mapperCount;
        pthread_create(&tid[mapperCount], &attr[mapperCount], mapper, count);
    }

    pthread_t reducerTid;
    pthread_attr_t reducerAttr;
    pthread_attr_init(&reducerAttr);
    pthread_create(&reducerTid, &reducerAttr, reducer, NULL);

    for(int i = 0; i < K; i++) {
        pthread_join(tid[i], NULL);
    }
    pthread_join(reducerTid, NULL);    

    for(int i = 0; i < K; i++) {
        free(splitFileNames[i]);
    }
    free(splitFileNames);

    printf("\n%s\n", "This Vector Will Be Written To Result File:");
    for(int i = 0; i < vectorSize; i++) {
        printf("%d %d\n", i + 1, resultVector[i]);
    }

    FILE *resultFile = fopen(resultFileName, "w");

    for(int i = 0; i < vectorSize; i++) {
        char toWrite[MAX_LINE_SIZE];
        if(resultVector[i] != 0) {
            snprintf(toWrite, MAX_LINE_SIZE,"%d %d\n", i + 1, resultVector[i]);
            fprintf(resultFile, "%s", toWrite);
        }
    }
    fclose(resultFile);

    free(sem_empty);
    free(sem_mutex);
    free(vectorValues);
    free(resultVector);

    return 0;
}

//thread functions
void *mapper(void *param) {
    int mapperNumber = *(int*)param;
    printf("%s%d%s\n", "Mapper ", mapperNumber, " started...");

    FILE *fp = fopen(splitFileNames[mapperNumber], "r");


    char *line = malloc(MAX_LINE_SIZE * sizeof(char));
    while(fgets(line, MAX_LINE_SIZE, fp) != NULL) {
        sem_wait(sem_empty[mapperNumber]);
        sem_wait(sem_mutex[mapperNumber]);
        push(buffers[mapperNumber], line);
        printf("%s%d%s%s\n", "WRITE TO BUF", mapperNumber, " : ",line);
        sem_post(sem_mutex[mapperNumber]);
        line = malloc(MAX_LINE_SIZE * sizeof(char));
        //This sleep makes mapper process take longer. Therefore concurrency is more visible.
        //Other than that it is completely redundant.
        usleep(50);
    }
    free(line);
    sem_wait(sem_empty[mapperNumber]);
    sem_wait(sem_mutex[mapperNumber]);
    push(buffers[mapperNumber], DONE);
    sem_post(sem_mutex[mapperNumber]);

    fclose(fp);

    //To prevent memory leak
    free(param);

    pthread_exit(0);
}

void *reducer(void *param) {
    printf("%s\n", "Reducer Started...");

    int completed = 0;
    while(!completed) {
        completed = 1;
        for(int i = 0; i < K; i++) {
            if(buffers[i] != NULL) {
                completed = 0;
                sem_wait(sem_mutex[i]);
                if(buffers[i]->size != 0) {

                    char* line = pop(buffers[i]);
                    sem_post(sem_mutex[i]);
                    sem_post(sem_empty[i]);

                    printf("%s%d%s%s\n", "READ FROM BUF", i, " : ",line);

                    int row, column, value;
                    if(strcmp( line, DONE)) {
                        sscanf(line, "%d %d %d", &row, &column, &value);
                        resultVector[row - 1] += value * vectorValues[column - 1];
                        free(line);
                    }
                    else {
                        
                        sem_close(sem_mutex[i]);
                        sem_close(sem_empty[i]);
                        free(sem_mutex[i]);
                        free(sem_empty[i]);
                        free(buffers[i]->lines);
                        free(buffers[i]);
                        buffers[i] = NULL;
                    }
                }
                else {
                    sem_post(sem_mutex[i]);
                }
            }
        }
    }

    free(buffers);
    pthread_exit(0);
}


//Helping Functions
void setValues(int argc, char *argv[]) {
    if( argc != 6) {
        printf("%s", "WRONG NUBMER OF ARGUMENTS!");
        exit(1);
    }
    matrixFileName = argv[1];
    vectorFileName = argv[2];
    resultFileName = argv[3];
    K = atoi(argv[4]);
    B = atoi(argv[5]);

    if(sizeof(K) != sizeof(int) || K > MAX_K || K < MIN_K) {
        printf("%s", "K VALUE IS NOT SUITABLE!");
        exit(1);
    }

    if(sizeof(B) != sizeof(int) || B > MAX_B || B < MIN_B) {
        printf("%s", "B VALUE IS NOT SUITABLE!");
        exit(1);
    }
}

char** generateSplitFiles(char* matrixFile, int splitSize) {
    FILE *fp = fopen(matrixFile, "r");

    //Creating the names of split files
    char **splitFileNames;

    splitFileNames = (char **)malloc(K * sizeof(char *)); 
    for (int i=0; i<K; i++) {
        splitFileNames[i] = (char *)malloc(FILE_NAME_SIZE * sizeof(char)); 
        snprintf(splitFileNames[i], FILE_NAME_SIZE, "Split %d", i + 1); // puts string into buffer
    }

    //Opening the split files
    FILE* filedescs[K];
    for(int i = 0; i < K; i++) {
        filedescs[i] = fopen(splitFileNames[i], "w");
    }

    char *currentLine = NULL;
    size_t len = 0, read;
    int fileDescCounter = 0, lineCounter = 0;
    short last = 0;
    while ((read = getline(&currentLine, &len, fp)) != -1) {

        if( lineCounter == splitSize && !last) {
            fileDescCounter++;
            lineCounter = 0;
            if(fileDescCounter == K - 1) {
                last = 1;
            }
        }
        
        fprintf(filedescs[fileDescCounter], "%s", currentLine);
        lineCounter++;
    }

    fclose(fp);
    for(int i = 0; i < K; i++) {
        fclose(filedescs[i]);
    }

    return splitFileNames;
}

int getLineCount(char *fileName) {
    FILE *fp = fopen(fileName, "r");
    if(fp == NULL) {
        printf("%s%s\n", fileName, " Not Found!");
        exit(1);
    }

    char *currentLine = NULL;
    size_t len = 0, read;

    int lineCount = 0;
    if(fileName == NULL) {
        printf("ERROR: FILE NAME IS NOT SPECIFIED\n");
        exit(1);
    }

    while ((read = getline(&currentLine, &len, fp)) != -1) {
        lineCount++;
    }

    fclose(fp);
    return lineCount;
}

//Buffer implentation
void push(struct Buffer *buffer, char* line) {
    if(buffer->size == buffer->capacity) {
        printf("%s\n", "Buffer Full!");
        return;
    }
    buffer->lines[buffer->end] = line;
    buffer->end = (buffer->end  + 1) % buffer -> capacity;
    buffer->size = buffer->size + 1;
}

char* pop(struct Buffer *buffer) {
    
    if(buffer->size == 0) {
        printf("%s\n", "Buffer Empty!");
        return NULL;
    }

    char *line = buffer->lines[buffer->front];
    buffer->front = (buffer->front + 1) % buffer -> capacity;
    buffer->size = buffer->size - 1;

    return line;
}

struct Buffer** getBuffers() {

    struct Buffer** buffers = malloc(K * sizeof(struct Buffer*));
    for(int i = 0; i < K; i++) {
        buffers[i] = malloc(sizeof(struct Buffer));
        buffers[i] -> capacity = B;
        buffers[i] -> size = 0;
        buffers[i] -> front = 0;
        buffers[i] -> end = 0;
        buffers[i] -> lines = malloc(B * sizeof(char*));
    }

    return buffers;
}
