#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>

int K;
int MAX_STR_LEN = 36;

int **partialResults;
char** splitFileNames;
char *matrixFileName;
char *vectorFileName;
char *resultFileName;

int getLineCount(char *fileName);
char** generateSplitFiles(char* matrixFile, int splitLength);
void *mapper(void *param);
void *reducer(void *param);
void printElapsedTime(struct timeval *start, struct timeval *end);

int main(int argc, char *argv[]) {
    struct timeval start, end;
    struct timeval startMapper, endMapper;
    gettimeofday(&start, NULL);

    matrixFileName = argv[1];
    vectorFileName = argv[2];
    resultFileName = argv[3];
    K = atoi(argv[4]);

    if(sizeof(K) != sizeof(int) || K > 10 || K <= 0) {
        printf("%s", "K VALUE IS NOT SUITABLE!");
        return 1;
    }

    int lineCount = getLineCount(matrixFileName);
    if(lineCount > 10000 || lineCount <= 0) {
        printf("%s", "N VALUE IS NOT SUITABLE!");
        return 1;
    }
    int splitLength = lineCount / K;

    splitFileNames = generateSplitFiles(matrixFileName, splitLength);

    int vectorLineCount = getLineCount(vectorFileName);

    partialResults = (int **)malloc(K * sizeof(int *)); 
    for (int i= 0 ; i < K; i++) {
        partialResults[i] = (int *)malloc(vectorLineCount * sizeof(int));
    }

    pthread_t tid[K];
    pthread_attr_t attr[K];

    gettimeofday(&startMapper, NULL);
    //Create as many threads as split files
    for(int childCount = 0; childCount < K; childCount++) {
        //Get the default attributes.
        pthread_attr_init(&attr[childCount]);
        //Create the thread
        int *childc = malloc(sizeof(int));
        *childc = childCount;
        pthread_create(&tid[childCount], &attr[childCount], mapper, childc);

    }

    // Waiting for child calculations to complete
    for(int i = 0; i < K; i++) {
        pthread_join(tid[i], NULL);
    }

    gettimeofday(&endMapper, NULL);
    printElapsedTime(&startMapper, &endMapper);

    pthread_t reducerTid;
    pthread_attr_t reducerAttr;

    pthread_attr_init(&reducerAttr);
    //Create the thread
    pthread_create(&reducerTid, &reducerAttr, reducer, NULL);

    pthread_join(reducerTid, NULL);

    gettimeofday(&end, NULL);
    printElapsedTime(&start, &end);

    return 0;
}

char** generateSplitFiles(char* matrixFile, int splitLength) {
    FILE *fp = fopen(matrixFile, "r");

    //Creating the names of split files
    char **splitFileNames;

    splitFileNames = (char **)malloc(K * sizeof(char *)); 
    for (int i=0; i<K; i++) {
        splitFileNames[i] = (char *)malloc(16 * sizeof(char)); 
        snprintf(splitFileNames[i], 16, "Split %d", i + 1); // puts string into buffer
    }

    //Opening the split files
    FILE* filedescs[K];
    for(int i = 0; i < K; i++) {
        filedescs[i] = fopen(splitFileNames[i], "w");
    }

    char *currentLine = NULL;
    size_t len = 0;
    ssize_t read;
    int fileDescCounter = 0;
    int lineCounter = 0;
    short last = 0;
    while ((read = getline(&currentLine, &len, fp)) != -1) {

        if( lineCounter == splitLength && !last) {
            fileDescCounter++;
            lineCounter = 0;
            if(fileDescCounter == K - 1) {
                last = 1;
            }
        }
        
        char* xx = currentLine;
        fprintf(filedescs[fileDescCounter], "%s", xx);
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

    char *currentLine = NULL;
    size_t len = 0;
    ssize_t read;

    int lineCount = 0;
    if(fileName == NULL) {
        printf("FAILED\n");
        exit(1);
    }

    while ((read = getline(&currentLine, &len, fp)) != -1) {
        lineCount++; 
    }

    return lineCount;
}

void * mapper(void *param) {
    FILE *fp;
    int lineCount = getLineCount(vectorFileName);
    //int *x = (int *)param;
    int childCount = *(int*)param;
    int vector[lineCount];

    //read vector file
    fp = fopen(vectorFileName, "r");
    
    int row;
    int column;
    int value;

    //It would have been more efficient to read vectorfiles outside  the mappers.
    //However, project description tells to read this way.
    while(fscanf( fp, "%d %d", &row, &value) != EOF) {
        if(row > lineCount || row <= 0) {
            printf("%s\n", "Invalid Vector File!");
        }
        vector[row - 1] = value;
    }

    fclose(fp);

    fp = fopen(splitFileNames[childCount], "r");

    //Getting rid of garbage values
    for(int counter = 0; counter < lineCount; counter++) {
        partialResults[childCount][counter] = 0;
    }

    //Generating the partial result array
    while(fscanf( fp, "%d %d %d", &row, &column, &value) != EOF) {
        partialResults[childCount][row - 1] += vector[column - 1] * value;
    }

    fclose(fp);
    pthread_exit(0);
}

void* reducer(void *param) {
    FILE *fp;

    int lineCount = getLineCount(vectorFileName);

    int result[lineCount];

    //Getting rid of the garbage values
    for(int i = 0; i < lineCount; i++) {
        result[i] = 0;
    }

    for(int i = 0; i < K; i++) {
            int *partialResult;
            partialResult = partialResults[i];
            for(int j = 0; j < lineCount; j++) {
                result[j] += partialResult[j];
            }

    }

    fp = fopen(resultFileName, "w");

    for(int row = 0; row < lineCount; row++) {
        char toWrite[MAX_STR_LEN];
        if(result[row] != 0) {
            snprintf(toWrite, MAX_STR_LEN,"%d %d\n", row + 1, result[row]);
            fprintf(fp, "%s", toWrite);
        }
    }
    fclose(fp);

    pthread_exit(0);
}

void printElapsedTime(struct timeval *start, struct timeval *end) {
    unsigned long elapsedTime =  ((end->tv_sec - start->tv_sec) * 1000000) + (end->tv_usec - start->tv_usec);

    printf("Time elapsed is %ld microseconds\n", elapsedTime);
}