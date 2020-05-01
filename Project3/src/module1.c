#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

extern char initData1[];
extern char initData2[];
extern char unutilized1[];
extern char unutilized2[];
extern void nextStep();

int main() {
    // Step 1
    printf("%s\n\n", "Step 1 Initiated");

    printf("%s%lx\n", "Address of main is ", (long unsigned int) main);
    printf("%s%lx\n", "Address of initialized data 1 (128B) is: ", (long unsigned int) initData1);
    printf("%s%lx\n", "Address of unutilized data 1 (16KB) is: ", (long unsigned int) unutilized1);
    printf("%s%lx\n", "Address of initialized data 2 (64B) is: ", (long unsigned int) initData2);
    printf("%s%lx\n", "Address of unutilized data 2 (8KB) is: ", (long unsigned int) unutilized2);
    printf("%s%lx\n", "Address of nextStep function is: ", (long unsigned int) nextStep);
    printf("%s%lx\n", "Address of printf function is: ", (long unsigned int) printf);
    
    printf("%s%d\n", "Process PID: ",getpid());
    nextStep();
    // Step 2

    void *data1 = malloc(512 * 1024);
    void *data2 = malloc(256 * 1024);
    void *data3 = malloc(256 * 1024);

    printf("%s%lx\n", "Address of data1: ", (long unsigned int) data1);
    printf("%s%lx\n", "Address of data2: ", (long unsigned int) data2);
    printf("%s%lx\n", "Address of data3: ", (long unsigned int) data3);
    nextStep();
    printf("%s\n\n", "Step 2 Initiated");
    nextStep();
    printf("%s\n\n", "Step 3 Initiated");
    nextStep();
    printf("%s\n\n", "Step 4 Initiated");
    nextStep();
    printf("%s\n\n", "Step 5 Initiated");
    nextStep();
    printf("%s\n\n", "Step 6 Initiated");
    printf("%s\n", "Enter n to exit");
    nextStep();
    return 0;//
}
