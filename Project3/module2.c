#include <stdio.h>
// Initialized data with size of 128 Bytes.
char initData1 [128] = {'O'};
// Uninitialized data, size 16KB.
char unutilized1 [16 * 1024];

extern void nextStep();

int factorial(int num) {
    if (num == 0) {
        return 1;
    }
    int local;

    if (num < 16) {
        printf("%s%lx\n", "Address of local variable: ",(unsigned long int) &local);
    }
    int result;
    result =  num * factorial(num - 1);
    return result;
}