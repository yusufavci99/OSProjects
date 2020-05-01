#include <stdio.h>

char initData [128];
char unutil [16 * 1024];

void nextStep();
int main() {
    for( int i = 0; i < 128; i++) {
        initData[i] = i + 1;
    }

    printf("%s%lx\n", "Address of main is ", (long unsigned int) main);
    printf("%s%lx\n", "Address of initialized date is: ", (long unsigned int) initData);
    printf("%s%lx\n", "Address of unutil date is: ", (long unsigned int) unutil);
    nextStep();

    return 0;
}

void nextStep() {
    char stepCheck = ' ';
    while(stepCheck != 'n') {
        stepCheck = getchar();
    }
}
