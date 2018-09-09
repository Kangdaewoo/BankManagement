#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "state.h"
#include "bank.h"
#include "utils.h"

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

unsigned int branchNumber = 0;
unsigned int accountNumber;

State *createState(int numCommands, char *message, int (*handler) ()) {
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
    welcomeState = createState(2, "Welcome!\nIf you have an account, press 1.\nIf you are new, press 2.\nIf you want to quit, press 0\n", NULL);
}

int signInHandler() {
    while (1) {
        unsigned int temp;
        scanf("%u", &temp);
        
        if (isValidAccountNumber(temp) == 1) {
            printf("Account number verified.\nMoving onto the main menu.\n");
            accountNumber = temp;
            return 0;
        } 
        
        printf("It's not a valid account number.\nPlease enter again.\n");
    }
}

void createSignIn() {
    signInState = createState(1, "Please provide your account number.\n", signInHandler);
}

int signUpHandler() {
    char name[MAX_NAME_LENGTH];
    char firstName[MAX_NAME_LENGTH];
    char lastName[MAX_NAME_LENGTH];
    scanf("%s %s", firstName, lastName);

    strcpy(name, firstName);
    strcat(name, " ");
    strcat(name, lastName);
    
    printf("Please provide desired password.\n");
    char password[MAX_NAME_LENGTH];
    scanf("%s", password);

    printf("Processing...\n");
    accountNumber = createAccount(name, password, 0);
    printf("This is your account number: %u\nPlease remember this.\n", accountNumber);

    return 0;
}

void createSignUp() {
    signUpState = createState(1, "Please provide your first and last name.\n", signUpHandler);
}

void createMain() {
    mainState = createState(6, "To see your account information, press 1.\nTo deposit, press 2.\nTo withdraw, press 3.\nTo make a transaction, press 4.\nTo view transactions, press 5.\nTo quit, press 6.\n", NULL);
}

int displayAccountInfoHandler() {
    char *accountInfo = getAccountInfo(accountNumber);
    printf("%s", accountInfo);
    free(accountInfo);
    return 0;
}

void createDisplayAccountInfo() {
    accountInfoState = createState(1, "", displayAccountInfoHandler);
}

int depositHandler() {
    double amount;
    scanf("%lf", &amount);

    char password[MAX_NAME_LENGTH];
    printf("Please provide the password.\n");
    scanf("%s", password);
    if (isCorrectPassword(accountNumber, password) == 0) {
        printf("Wrong password.\nReturning to the main menu.\n");
        return 0;
    }
    printf("Password correct.\n");

    int result = deposit(accountNumber, branchNumber, amount);
    if (result == -1) {
        printf("Sorry, not enough money.\n");
    } else {
        printf("Successfully deposited.\n");
    }
    return 0;
}

void createDeposit() {
    depositState = createState(1, "Please provide how much you want to deposit.\n", depositHandler);
}

int withdrawHandler() {
    double amount;
    scanf("%lf", &amount);

    char password[MAX_NAME_LENGTH];
    printf("Please provide the password.\n");
    scanf("%s", password);
    if (isCorrectPassword(accountNumber, password) == 0) {
        printf("Wrong password.\nReturning to the main menu.\n");
        return 0;
    }
    printf("Password correct.\n");

    int result = withdraw(accountNumber, branchNumber, amount);
    if (result == -1) {
        printf("Sorry, not enough money.\n");
    } else {
        printf("Successfully withdrawed.\n");
    }
    return 0;
}

void createWithdraw() {
    withdrawState = createState(1, "Please provide how much you want to withdraw.\n", withdrawHandler);
}

int transactHandler() {
    unsigned int accountNum;
    scanf("%u", &accountNum);

    printf("Please provide how much you want to send.\n");
    double amount;
    scanf("%lf", &amount);

    char password[MAX_NAME_LENGTH];
    printf("Please provide the password.\n");
    scanf("%s", password);
    if (isCorrectPassword(accountNumber, password) == 0) {
        printf("Wrong password.\nReturning to the main menu.\n");
        return 0;
    }
    printf("Password correct.\n");

    int result = transact(accountNumber, accountNum, amount);
    if (result == -1) {
        printf("Sorry, not enough money.\n");
    } else {
        printf("Transaction successful.\n");
    }
    return 0;
}

void createTransact() {
    transactState = createState(1, "Please provide the account number of the customer you want to send to\n", transactHandler);
}

int viewTransactionsHandler() {
    Transaction *transaction = getTransactions(accountNumber);
    if (transaction == NULL) {
        printf("You have not made any transactions.\n");
        return 0;
    }
    while (1) {
        printf("%s\n", transaction->memo);
        if (transaction->from == accountNumber) {
            transaction = transaction->senderParent;
        } else {
            transaction = transaction->receiverParent;
        }

        if (transaction == NULL) {
            printf("\nNo more transactions.\nReturning to the main menu.\n");
            return 0;
        }

        printf("\nPress 1 to view more transactions or 0 to return to the main menu.\n");
        int command;
        scanf("%d", &command);
        if (command == 0) {
            return 0;
        }
    }
    return 0;
}

void createViewTransactions() {
    viewTransactionsState = createState(1, "", viewTransactionsHandler);
}

int signOutHandler() {
    accountNumber = 0;
    return 0;
}

void createSignOut() {
    signOutState = createState(1, "Thank you and have a wonderful day.\n", signOutHandler);
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

void stateMachine() {
    State *currentState = welcomeState;
    while (1) {
        printf("%s", currentState->message);

        if (currentState->handler != NULL) {
            int nextState = currentState->handler();
            currentState = currentState->next[nextState];
        } else {
            int command;
            scanf("%d", &command);

            if (command - 1 == -1) {
                printf("Good bye!\n");
                exit(0);
            }

            if (command < 1 || command > currentState->numCommands) {
                printf("Wrong command!\n\n");
                continue;
            }
            currentState = currentState->next[command - 1];
        }
        printf("\n");
    }
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