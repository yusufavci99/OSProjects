#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>


int MAX_STR_LEN = 36;
int K;

int getLineCount(char *fileName);
char** generateSplitFiles(char* matrixFile, int splitLength);
void printElapsedTime(struct timeval *start, struct timeval *end);

int main(int argc, char *argv[]) {
    struct timeval start, end;
    struct timeval startMapper, endMapper;

    gettimeofday(&start, NULL);

    char *matrixFileName = argv[1];
    char *vectorFileName = argv[2];
    char *resultFileName = argv[3];
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

    char** splitFileNames = generateSplitFiles(matrixFileName, splitLength);

    pid_t pid;

    gettimeofday(&startMapper, NULL);
    //Create as many childs as split files
    for(int childCount = 0; childCount < K; childCount++) {
        pid = fork();
        if(pid < 0) {
            printf("FAIL\n");
        }

        // MAPPPER
        else if(pid == 0) { // Child process
            FILE *fp;
            int lineCount = getLineCount(vectorFileName);

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
                    return -1;
                }
                vector[row - 1] = value;
            }

            fclose(fp);

            fp = fopen(splitFileNames[childCount], "r");


            int partialResult[lineCount];
            //Getting rid of garbage values
            for(int counter = 0; counter < lineCount; counter++) {
                partialResult[counter] = 0;
            }
            
            //Generating the partial result array
            while(fscanf( fp, "%d %d %d", &row, &column, &value) != EOF) {
                partialResult[row - 1] += vector[column - 1] * value;
            }

            char interFileName[MAX_STR_LEN];
            snprintf(interFileName, MAX_STR_LEN, "Intermediate File %d", childCount + 1);
            fclose(fp);
            fp = fopen(interFileName, "w");

            for(int r = 0; r < lineCount; r++) {
                char toWrite[MAX_STR_LEN];
                if(partialResult[r] != 0) {
                    snprintf(toWrite, MAX_STR_LEN,"%d %d\n", r + 1, partialResult[r]);
                    fprintf(fp, "%s", toWrite);
                }
            }
            exit(0);
        }

    }

    // Waiting for child calculations to complete
    for(int i = 0; i < K; i++) {
        wait(0);
    }

    gettimeofday(&endMapper, NULL);
    printElapsedTime(&startMapper, &endMapper);

    pid = fork();
    if(pid < 0) {
        printf("FORK FAILED\n");
        return -1;
    }

    // REDUCER
    else if(pid == 0) { // Child process
        FILE* fp; 

        int lineCount = getLineCount(vectorFileName);

        int result[lineCount];

        //Getting rid of the garbage values
        for(int i = 0; i < lineCount; i++) {
            result[i] = 0;
        }


        for(int i = 0; i < K; i++) {
            char interFileName[MAX_STR_LEN];
            snprintf(interFileName, MAX_STR_LEN, "Intermediate File %d", i + 1);

            fp = fopen(interFileName, "r");

            int row;
            int value;

            while(fscanf( fp, "%d %d", &row, &value) != EOF) {
                result[row - 1] += value;
            }

        }

        fclose(fp);
        fp = fopen(resultFileName, "w");

        for(int row = 0; row < lineCount; row++) {
            char toWrite[MAX_STR_LEN];
            if(result[row] != 0) {
                snprintf(toWrite, MAX_STR_LEN,"%d %d\n", row + 1, result[row]);
                fprintf(fp, "%s", toWrite);
            }
        }
        fclose(fp);
        exit(0);
    }

    wait(0);

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

void printElapsedTime(struct timeval *start, struct timeval *end) {
    unsigned long elapsedTime =  ((end->tv_sec - start->tv_sec) * 1000000) + (end->tv_usec - start->tv_usec);

    printf("Time elapsed is %ld microseconds\n", elapsedTime);
}