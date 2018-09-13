#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "bank.h"
#include "state.h"
#include "server.h"
#include "string.h"


int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: please indicate if \"server\" or \"console\"\n");
        exit(1);
    }

    initializeBank();
    initializeStates();

    if (strcmp(argv[1], "server") == 0) {
        startServer();
    } else if (strcmp(argv[1], "console") == 0) {
        int fds[2] = {STDIN_FILENO, STDOUT_FILENO};
        stateMachine(fds);
    }

    return 0;
}