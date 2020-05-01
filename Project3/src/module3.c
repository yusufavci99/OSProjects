#include <stdio.h>
// Waits until n is pressed.
void nextStep() {
    char stepCheck = ' ';
    while(stepCheck != 'n') {
        stepCheck = getchar();
    }
}