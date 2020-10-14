#define RANDOM_ACCESS_COUNT 900

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>

unsigned long getElapsedTime(struct timeval *start, struct timeval *end);

int main (int argc, char *argv[]) {
    srand((unsigned int)(time(NULL)));
    struct timeval start, end;

    // Argument count control
    if (argc != 3) {
        printf ("Wrong number of arguments entered!\n");
        return 1;
    }

    // K validity check
    int K = atoi(argv[1]);
    if (K <= 0) {
        printf("%s\n", "Please gave a positive integer for K");
        return 1;
    }

    // Opening the file
    char *F = argv[2];
    int fd = open(F, O_RDONLY);
    if (fd < 0) {
        printf("%s%s\n", "Could not find (or open) a file named: ", F);
        return 1;
    }

    size_t file_size;
    struct stat file_info;
    if (stat(F ,&file_info) == 0) {
        file_size = (size_t)file_info.st_size;
    }
    else {
        printf("%s\n", "Couldn't determine the file size.");
        return 1;
    }
    printf("%s%ld%s\n", "File size: ", file_size, " B");

    if(K > file_size) {
        printf("%s\n", "K bigger than file size");
        return 1;
    }
    // Make random accesses
    unsigned long elapsed_time = 0;
    unsigned long read_time;
    char buffer[K];
    printf("%s\n", "Starting Random Access");
    for(int read_count = 0; read_count < RANDOM_ACCESS_COUNT; read_count++) {

        // Get a random index
        size_t index = (double) rand() / RAND_MAX * (file_size - K + 1);    
        
        // Set fd
        lseek(fd, index, SEEK_SET);
        
        // Calculate read time
        gettimeofday(&start, NULL);
        read(fd, buffer, K);
        gettimeofday(&end, NULL);        
        read_time = getElapsedTime(&start, &end);
        elapsed_time += read_time;
        printf("%s%d\n", "Read Count: ", read_count);
        printf("%s%ld\n", "Read From Index: ", index);
        printf("%s%ld%s\n", "Read Time: ", read_time, " microseconds");
        printf("%s\n", "Read: ");
        for(int j = 0; j < K; j++) {
            printf("%c", buffer[j]);
        }
        printf("\n\n");
    }
    printf("%s\n", "Random Access Done");

    unsigned long average_time = elapsed_time / RANDOM_ACCESS_COUNT;
    printf("%s%ld%s\n", "Total elapsed time: ",elapsed_time, " microseconds");
    printf("%s%ld%s\n", "Average access time: ",average_time, " microseconds");

    close(fd);
    return 0;
}

unsigned long getElapsedTime(struct timeval *start, struct timeval *end) {
    return ((end->tv_sec - start->tv_sec) * 1000000) + (end->tv_usec - start->tv_usec);
}