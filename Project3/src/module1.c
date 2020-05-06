#define PAGESIZE 4096

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>

extern char initData1[];
extern char initData2[];
extern char unutilized1[];
extern char unutilized2[];
extern void nextStep();
extern int factorial();

int main() {
    // Step 1
    printf("%s\n\n", "Step 1 Initiated");

    printf("%s%lx\n", "Address of main is ", (long unsigned int) main);
    printf("%s%lx\n", "Address of initialized data 1 (128B) is: ", (long unsigned int) initData1);
    printf("%s%lx\n", "Address of unutilized data 1 (16KB) is: ", (long unsigned int) unutilized1);
    printf("%s%lx\n", "Address of initialized data 2 (64B) is: ", (long unsigned int) initData2);
    printf("%s%lx\n", "Address of unutilized data 2 (8KB) is: ", (long unsigned int) unutilized2);
    printf("%s%lx\n", "Address of nextStep function is: ", (long unsigned int) nextStep);
    printf("%s%lx\n", "Address of factorial function is: ", (long unsigned int) factorial);
    printf("%s%lx\n", "Address of printf function is: ", (long unsigned int) printf);
    
    printf("%s%d\n", "Process PID: ",getpid());

    nextStep();
    printf("%s\n\n", "Step 2 Initiated");
    // Step 2

    void *heap_data1 = malloc(512 * 1024);
    void *heap_data2 = malloc(256 * 1024);
    void *heap_data3 = malloc(256 * 1024);
    void *heap_data4 = malloc(1024 * 1024);

    printf("%s%lx\n", "Address of heap data1: ", (long unsigned int) heap_data1);
    printf("%s%lx\n", "Address of heap data2: ", (long unsigned int) heap_data2);
    printf("%s%lx\n", "Address of heap data3: ", (long unsigned int) heap_data3);
    printf("%s%lx\n", "Address of heap data4: ", (long unsigned int) heap_data4);
    
    nextStep();
    printf("%s\n\n", "Step 3 Initiated");

    // stack size doesn't shrink https://stackoverflow.com/questions/8203110/will-the-stack-of-a-c-program-ever-shrink
    printf("%s%d\n", "Fact: ", factorial(1024 * 32));
  
    nextStep();
    printf("%s\n\n", "Step 4 Initiated");

    printf("%s\n", "Creating File!");
    

    int fd = open("project_file", O_RDWR | O_CREAT, 0600);
    printf("%s\n", "Created file with name: project_file");

    for(int i = 0; i < 1024 * 32; i++) {
        write(fd, "abcdefghijklmnopqrstuvwxyz12345\n", 32);
    }

    printf("%s\n", "Wrote 1MB data to project_file");

    void *memmap = mmap(NULL, 1024 * 1024, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    printf("%s%lx\n", "Address of memory map: ", (long unsigned int) memmap);

    nextStep();
    printf("%s\n\n", "Step 5 Initiated");

    for (int i = 0; i < 1024 * 512; i++) {
        ((char *)memmap)[i] = i % 128 + 40;
    }

    printf("%s\n", "Reading some values from the start of the mmap");
    for (int i = 0; i < 64; i++) {
        printf("%c", ((char *)memmap)[i] );
    }

    printf("\n%s\n", "Reading some values from the end of the mmap");
    for (int i = 1024 * 1024 - 64; i < 1024 * 1024; i++) {
        printf("%c", ((char *)memmap)[i] );
    }

    close(fd);
    nextStep();
    printf("%s\n\n", "Step 6 Initiated");

    unsigned char *mainPage = (unsigned char *) (PAGESIZE * ((long unsigned int) main / PAGESIZE));

    for( int i = 0; i < PAGESIZE; i += 1) {
        printf("%s%lx ", "Address: ", (long unsigned int) (mainPage + i));
        printf("%s%02x\n", "Data   : ", mainPage[i]);
    }

    printf("%s\n", "Enter n to exit");
    nextStep();
    return 0;//
}
