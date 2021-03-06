#ifndef MAX_BUF_LENGTH
#define MAX_BUF_LENGTH 120
#endif


typedef struct state {
    int numCommands;
    const char *message;
    int (*handler) (int inputFd, int outputFd);
    struct state *previous;
    struct state **next;
} State;

/**
 * States:
 *      1. Welcome state:
 *          - Sign in state
 *          - Sign up state
 *      2. Sign in state:
 *          - Main state
 *      3. Sign up state:
 *          - Welcome state
 *      4. Main state:
 *          - Account information state
 *          - Deposit state
 *          - Withdraw state
 *          - Transact state
 *          - Sign out state
 *      5. Account information state:
 *          - Main state
 *      6. Deposit state:
 *          - Main state
 *      7. Withdraw state:
 *          - Main state
 *      8. Transact state:
 *          - Main state
 *      9. View transactions state:
 *          - Main state
 *      9. Sign out state:
 *          - Welcome state
 * */

void initializeStates();
void *stateMachine(void *fds);