#define BLOCK_SIZE 4096
#define FILE_NAME "p1_file"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

void write_block(int fd);

int main(int argc, char *argv[]) {

    // Argument count control
    if (argc != 2) {
        printf ("Wrong number of arguments entered!\n");
        return 1;
    }

    // Argument validity control
    int N = atoi(argv[1]);
    if (N <= 0) {
        printf("%s\n", "Please gave a positive integer as block number.");
        return 1;
    }

    // File creation
    printf("%s%s\n", "Created file with name: ", FILE_NAME);
    int fd = open(FILE_NAME, O_RDWR | O_CREAT | O_TRUNC, 0600);
    if (fd < 0) {
        printf("%s\n", "Could not create the file.");
        return 1;
    }

    // Writing
    printf("%s%d%s\n", "Writing ", N, " blocks.");
    for( int i = 0; i < N; i++) {
        write_block(fd);
    }

    // Closing
    printf("%s\n", "Done Writing");
    close(fd);
    return 0;
}

void write_block(int fd) {

    size_t length = BLOCK_SIZE;
    char str[BLOCK_SIZE];
    char *dest = &str[0];

    while (length-- > 0) {
        *dest++ = '0';
    }
    *dest = '\0';

    write(fd, str, BLOCK_SIZE);
}
