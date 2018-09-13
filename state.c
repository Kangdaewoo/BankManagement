#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "state.h"
#include "bank.h"
#include "utils.h"


const char *WELCOME_MSG = "Welcome!\nIf you have an account, press 1.\nIf you are new, press 2.\nIf you want to quit, press 0\n";
const char *INVALID_ACCOUNT_NUM_MSG = "Account number verified.\nMoving onto the main menu.\n";
const char *INVALID_ACCOUNT_MSG = "It's not a valid account number.\nPlease enter again.\n";
const char *SIGN_IN_MSG = "Please provide your account number.\n";
const char *PROVIDE_PASSWORD_MSG = "Please provide password.\n";
const char *ACCOUNT_CREATED_MSG = "This is your account number: %u\nPlease remember this.\n";
const char *PROVIDE_NAME_MSG = "Please provide your first and last name.\n";
const char *MAIN_MSG = "To see your account information, press 1.\nTo deposit, press 2.\nTo withdraw, press 3.\nTo make a transaction, press 4.\nTo view transactions, press 5.\nTo quit, press 6.\n";
const char *WRONG_PASSWORD_MSG = "Wrong password.\nReturning to the main menu.\n";
const char *CORRECT_PASSWORD_MSG = "Password correct.\n";
const char *NOT_ENOUGH_MONEY_MSG = "Sorry, not enough money.\n";
const char *DEPOSIT_SUCCESSFUL_MSG = "Successfully deposited.\n";
const char *DEPOSIT_MSG = "Please provide how much you want to deposit.\n";
const char *WITHDRAW_SUCCESSFUL_MSG = "Successfully withdrawed.\n";
const char *PROVIDE_HOW_MUCH_TO_SEND_MSG = "Please provide how much you want to send.\n";
const char *TRANSACTION_SUCCESSFUL_MSG = "Transaction successful.\n";
const char *TRANSACT_MSG = "Please provide the account number of the customer you want to send to\n";
const char *NO_TRANSACTIONS_MSG = "You have not made any transactions.\n";
const char *NO_MORE_TRANSACTIONS_MSG = "\nNo more transactions.\nReturning to the main menu.\n";
const char *MORE_TRANSACTIONS_MSG = "\nPress 1 to view more transactions or 0 to return to the main menu.\n";
const char *SIGN_OUT_MSG = "Thank you and have a wonderful day.\n";
const char *GOOD_BYE_MSG = "Good bye!\n";
const char *WRONG_COMMAND_MSG = "Wrong command!\n\n";
const char *LINE_BREAKER_MSG = "\n";

unsigned int branchNumber = 0;
unsigned int accountNumber;

State *welcomeState;
State *signInState;
State *signUpState;
State *mainState;
State *accountInfoState;
State *depositState;
State *withdrawState;
State *transactState;
State *viewTransactionsState;
State *signOutState;

State *createState(int numCommands, const char *message, int (*handler) (int intputFd, int outputFd)) {
    State *newState = malloc(sizeof(State));
    checkNull(newState);

    newState->numCommands = numCommands;
    newState->message = message;
    newState->handler = handler;

    newState->next = malloc(sizeof(State *) * numCommands);
    checkNull(newState->next);

    return newState;
}

void createWelcome() {
    welcomeState = createState(2, WELCOME_MSG, NULL);
}

int signInHandler(int inputFd, int outputFd) {
    while (1) {
        char buf[MAX_BUF_LENGTH];
        readFromFd(inputFd, buf, MAX_BUF_LENGTH - 1);
        unsigned int temp = strtoul(buf, NULL, 10);
        
        if (isValidAccountNumber(temp) == 1) {
            write(outputFd, INVALID_ACCOUNT_NUM_MSG, strlen(INVALID_ACCOUNT_NUM_MSG));
            accountNumber = temp;
            return 0;
        } 
        
        write(outputFd, INVALID_ACCOUNT_MSG, strlen(INVALID_ACCOUNT_MSG));
    }
}

void createSignIn() {
    signInState = createState(1, SIGN_IN_MSG, signInHandler);
}

int signUpHandler(int inputFd, int outputFd) {
    char name[MAX_BUF_LENGTH];
    readFromFd(inputFd, name, MAX_BUF_LENGTH - 1);
    
    write(outputFd, PROVIDE_PASSWORD_MSG, strlen(PROVIDE_PASSWORD_MSG));
    char password[MAX_BUF_LENGTH];
    readFromFd(inputFd, password, MAX_BUF_LENGTH - 1);

    accountNumber = createAccount(name, password, 0);
    char buf[MAX_BUF_LENGTH];
    sprintf(buf, ACCOUNT_CREATED_MSG, accountNumber);
    write(outputFd, buf, strlen(buf));

    return 0;
}

void createSignUp() {
    signUpState = createState(1, PROVIDE_NAME_MSG, signUpHandler);
}

void createMain() {
    mainState = createState(6, MAIN_MSG, NULL);
}

int displayAccountInfoHandler(int inputFd, int outputFd) {
    char *accountInfo = getAccountInfo(accountNumber);
    write(outputFd, accountInfo, strlen(accountInfo));
    free(accountInfo);
    return 0;
}

void createDisplayAccountInfo() {
    accountInfoState = createState(1, "", displayAccountInfoHandler);
}

int depositHandler(int inputFd, int outputFd) {
    char buf[MAX_BUF_LENGTH];
    readFromFd(inputFd, buf, MAX_BUF_LENGTH - 1);
    double amount = strtod(buf, NULL);

    char password[MAX_BUF_LENGTH];
    write(outputFd, PROVIDE_PASSWORD_MSG, strlen(PROVIDE_PASSWORD_MSG));
    readFromFd(inputFd, password, MAX_BUF_LENGTH - 1);

    if (isCorrectPassword(accountNumber, password) == 0) {
        write(outputFd, WRONG_PASSWORD_MSG, strlen(WRONG_PASSWORD_MSG));
        return 0;
    }
    write(outputFd, CORRECT_PASSWORD_MSG, strlen(CORRECT_PASSWORD_MSG));

    int result = deposit(accountNumber, branchNumber, amount);
    if (result == -1) {
        write(outputFd, NOT_ENOUGH_MONEY_MSG, strlen(NOT_ENOUGH_MONEY_MSG));
    } else {
        write(outputFd, DEPOSIT_SUCCESSFUL_MSG, strlen(DEPOSIT_SUCCESSFUL_MSG));
    }
    return 0;
}

void createDeposit() {
    depositState = createState(1, DEPOSIT_MSG, depositHandler);
}

int withdrawHandler(int inputFd, int outputFd) {
    char buf[MAX_BUF_LENGTH];
    readFromFd(inputFd, buf, MAX_BUF_LENGTH - 1);
    double amount = strtod(buf, NULL);

    write(outputFd, PROVIDE_PASSWORD_MSG, strlen(PROVIDE_PASSWORD_MSG));
    char password[MAX_BUF_LENGTH];
    readFromFd(inputFd, password, MAX_BUF_LENGTH - 1);

    if (isCorrectPassword(accountNumber, password) == 0) {
        write(outputFd, WRONG_PASSWORD_MSG, strlen(WRONG_PASSWORD_MSG));
        return 0;
    }
    write(outputFd, CORRECT_PASSWORD_MSG, strlen(CORRECT_PASSWORD_MSG));

    int result = withdraw(accountNumber, branchNumber, amount);
    if (result == -1) {
        write(outputFd, NOT_ENOUGH_MONEY_MSG, strlen(NOT_ENOUGH_MONEY_MSG));
    } else {
        write(outputFd, WITHDRAW_SUCCESSFUL_MSG, strlen(WITHDRAW_SUCCESSFUL_MSG));
    }
    return 0;
}

void createWithdraw() {
    withdrawState = createState(1, "Please provide how much you want to withdraw.\n", withdrawHandler);
}

int transactHandler(int inputFd, int outputFd) {
    char buf[MAX_BUF_LENGTH];
    readFromFd(inputFd, buf, MAX_BUF_LENGTH - 1);
    unsigned int accountNum = strtoul(buf, NULL, 10);

    write(outputFd, PROVIDE_HOW_MUCH_TO_SEND_MSG, strlen(PROVIDE_HOW_MUCH_TO_SEND_MSG));
    readFromFd(inputFd, buf, MAX_BUF_LENGTH - 1);
    double amount = strtod(buf, NULL);

    char password[MAX_BUF_LENGTH];
    write(outputFd, PROVIDE_PASSWORD_MSG, strlen(PROVIDE_PASSWORD_MSG));
    readFromFd(inputFd, password, MAX_BUF_LENGTH - 1);

    if (isCorrectPassword(accountNumber, password) == 0) {
        write(outputFd, WRONG_PASSWORD_MSG, strlen(WRONG_PASSWORD_MSG));
        return 0;
    }
    write(outputFd, CORRECT_PASSWORD_MSG, strlen(CORRECT_PASSWORD_MSG));

    int result = transact(accountNumber, accountNum, amount);
    if (result == -1) {
        write(outputFd, NOT_ENOUGH_MONEY_MSG, strlen(NOT_ENOUGH_MONEY_MSG));
    } else {
        write(outputFd, TRANSACTION_SUCCESSFUL_MSG, strlen(TRANSACTION_SUCCESSFUL_MSG));
    }
    return 0;
}

void createTransact() {
    transactState = createState(1, TRANSACT_MSG, transactHandler);
}

int viewTransactionsHandler(int inputFd, int outputFd) {
    Transaction *transaction = getTransactions(accountNumber);
    if (transaction == NULL) {
        write(outputFd, NO_TRANSACTIONS_MSG, strlen(NO_TRANSACTIONS_MSG));
        return 0;
    }

    while (1) {
        write(outputFd, transaction->memo, strlen(transaction->memo));
        if (transaction->from == accountNumber) {
            transaction = transaction->senderParent;
        } else {
            transaction = transaction->receiverParent;
        }

        if (transaction == NULL) {
            write(outputFd, NO_MORE_TRANSACTIONS_MSG, strlen(NO_MORE_TRANSACTIONS_MSG));
            return 0;
        }

        write(outputFd, MORE_TRANSACTIONS_MSG, strlen(MORE_TRANSACTIONS_MSG));
        char buf[MAX_BUF_LENGTH];
        readFromFd(inputFd, buf, MAX_BUF_LENGTH - 1);
        int command = strtol(buf, NULL, 10);
        // Stop showing transactions.
        if (command == 0) {
            return 0;
        }
    }
    return 0;
}

void createViewTransactions() {
    viewTransactionsState = createState(1, "", viewTransactionsHandler);
}

int signOutHandler(int inputFd, int outputFd) {
    accountNumber = 0;
    return 0;
}

void createSignOut() {
    signOutState = createState(1, SIGN_OUT_MSG, signOutHandler);
}

void assignNextStates();

void initializeStates() {
    branchNumber = createAccount("Branch Console", "0000", 1);

    createWelcome();
    createSignIn();
    createSignUp();
    createMain();
    createDisplayAccountInfo();
    createDeposit();
    createWithdraw();
    createTransact();
    createSignOut();
    createViewTransactions();

    assignNextStates();
}

/**
 * Server: multi-threaded
 *   Runs in a separate thread from the server.
 *   Directly writes to the client fd.
 *   Indirectly hears from clients through the server.
 * Console: single-threaded
 *   Writes to and reads from standard output and input.
 * */
void *stateMachine(void *fds) {
    int inputFd = ((int *) fds)[0];
    int outputFd = ((int *) fds)[1];
    State *currentState = welcomeState;
    while (1) {
        write(outputFd, currentState->message, strlen(currentState->message));

        if (currentState->handler != NULL) {
            int nextState = currentState->handler(inputFd, outputFd);
            currentState = currentState->next[nextState];
        } else {
            char buf[MAX_BUF_LENGTH];
            readFromFd(inputFd, buf, MAX_BUF_LENGTH - 1);
            int command = strtol(buf, NULL, 10);
            if (command - 1 == -1) {
                write(outputFd, GOOD_BYE_MSG, strlen(GOOD_BYE_MSG));
                break;
            }

            if (command < 1 || command > currentState->numCommands) {
                write(outputFd, WRONG_COMMAND_MSG, strlen(WRONG_COMMAND_MSG));
                continue;
            }
            currentState = currentState->next[command - 1];
        }
        write(outputFd, LINE_BREAKER_MSG, strlen(LINE_BREAKER_MSG));
    }

    // Let client know they can close. When they close connection, server will free/remove the client.
    if (outputFd != STDOUT_FILENO && inputFd != STDIN_FILENO) {
        write(outputFd, "You can now close the connection.\n", 34);
    }
    return NULL;
}

void assignNextStates() {
    welcomeState->next[0] = signInState;
    welcomeState->next[1] = signUpState;

    signInState->next[0] = mainState;

    signUpState->next[0] = welcomeState;

    mainState->next[0] = accountInfoState;
    mainState->next[1] = depositState;
    mainState->next[2] = withdrawState;
    mainState->next[3] = transactState;
    mainState->next[4] = viewTransactionsState;
    mainState->next[5] = signOutState;

    accountInfoState->next[0] = mainState;

    depositState->next[0] = mainState;

    withdrawState->next[0] = mainState;

    transactState->next[0] = mainState;

    viewTransactionsState->next[0] = mainState;

    signOutState->next[0] = welcomeState;
}