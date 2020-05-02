#include <stdio.h>
// Initialized data with size of 128 Bytes.
char initData1 [128] = {'O'};
// Uninitialized data, size 16KB.
char unutilized1 [16 * 1024];
// Initialized data, size 64 Bytes.
char initData2 [64] = {'S'};
// Uninitialized data, size 8KB.
char unutilized2 [8 * 1024];


extern void nextStep();

int factorial(int num) {
    if (num == 0) {
        nextStep();
        return 1;
    }
    int dummy;
    printf("%lx\n", (unsigned long int) &dummy);
    int result;
    result =  num * factorial(num - 1);
    return result;
}