#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "bank.h"
#include "state.h"
#include "server.h"


int main() {
    initializeBank();
    initializeStates();

    // int fds[2] = {STDIN_FILENO, STDOUT_FILENO};
    // stateMachine(fds);

    startServer();

    return 0;
}