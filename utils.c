#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


void checkNull(void *address) {
    if (address == NULL) {
        perror("malloc");
        exit(1);
    }
}

void readFromFd(int inputFd, char *buf, int length) {
    int numRead = read(inputFd, buf, length);
    buf[numRead] = '\0';
}