#include <stdlib.h>
#include <stdio.h>


void checkNull(void *address) {
    if (address == NULL) {
        perror("malloc");
        exit(1);
    }
}