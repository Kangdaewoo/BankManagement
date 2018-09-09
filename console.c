#include <stdlib.h>
#include <stdio.h>

#include "bank.h"
#include "state.h"


int main() {
    initializeBank();
    initializeStates();

    stateMachine();

    return 0;
}