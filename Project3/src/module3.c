#include <stdio.h>
// Initialized data, size 64 Bytes.
char initData2 [64] = {'S'};
// Uninitialized data, size 8KB.
char unutilized2 [8 * 1024];


// Waits until n is pressed.
void nextStep() {
    char stepCheck = ' ';
    while(stepCheck != 'n') {
        stepCheck = getchar();
    }
}