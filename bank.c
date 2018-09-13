#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "bank.h"
#include "utils.h"


int makeTransaction(unsigned int from, unsigned int to, double amount, char *memo);
int send(Account *sender, double amount);
int receive(Account *receiver, double amount);

Account *accounts[10000];
unsigned int customerIndex;
unsigned int branchIndex;
pthread_mutex_t accountsLock;

void initializeBank() {
    customerIndex = MAX_BRANCH_NUM + 1;
    branchIndex = 0;
    pthread_mutex_init(&accountsLock, NULL);
}

/**
 * 1 = valid, 0 = invalid.
 * */
int isValidAccountNumber(unsigned int accountNum) {
    if (accountNum > MAX_BRANCH_NUM && accountNum < customerIndex) {
        return 1;
    } else if (accountNum < branchIndex) {
        return 1;
    }
    return 0;
}

/**
 * Account info in string.
 * Must be freed after used.
 * */
char *getAccountInfo(unsigned int accountNum) {
    char *toReturn = malloc(sizeof(char) * MAX_MEMO_LENGTH);
    checkNull(toReturn);

    Account *account = findAccount(accountNum);
    sprintf(toReturn, "Name: %s\nAmount: $%.2lf\n", account->name, account->amount);
    return toReturn;
}

unsigned int generateAccountNum(char *name, int isBranch) {
    if (isBranch == 0) {
        pthread_mutex_lock(&accountsLock);
        customerIndex++;
        pthread_mutex_unlock(&accountsLock);
        return customerIndex - 1;
    } else {
        pthread_mutex_lock(&accountsLock);
        branchIndex++;
        pthread_mutex_unlock(&accountsLock);
        return branchIndex - 1;
    }
}

Account *findAccount(unsigned int accountNum) {
    return accounts[accountNum];
}

unsigned int createAccount(char *name, char *password, int isBranch) {
    unsigned int accountNum = generateAccountNum(name, isBranch);

    Account *newAccount = malloc(sizeof(Account));
    checkNull(newAccount);

    strcpy(newAccount->name, name);
    strcpy(newAccount->password, password);

    if (isBranch == 1) {
        newAccount->amount = 10000.0;
    } else {
        newAccount->amount = 0;
    }

    newAccount->lastTransaction = NULL;
    pthread_mutex_init(&(newAccount->lock), NULL);

    accounts[accountNum] = newAccount;

    return accountNum;
}

Transaction *createTransaction(Account *sender, Account *receiver, double amount, char *memo) {
    Transaction *newTransaction = malloc(sizeof(Transaction));
    checkNull(newTransaction);

    newTransaction->from = sender->number;
    newTransaction->to = receiver->number;
    newTransaction->amount = amount;
    newTransaction->memo = memo;
    newTransaction->senderParent = sender->lastTransaction;
    newTransaction->receiverParent = receiver->lastTransaction;
    sender->lastTransaction = newTransaction;
    receiver->lastTransaction = newTransaction;

    return newTransaction;
}

/**
 * Returns 0 if successful, -1 otherwise.
 * */
int deposit(unsigned int accountNum, unsigned int branchNum, double amount) {
    char *memo = malloc(sizeof(char) * (MAX_MEMO_LENGTH + 1));
    checkNull(memo);
    sprintf(memo, "Deposited $%.2lf at the branch %x", amount, branchNum);
    memo[MAX_MEMO_LENGTH] = '\0';

    int result = makeTransaction(branchNum, accountNum, amount, memo);
    return result;
}

/**
 * Returns 0 if successful, -1 otherwise.
 * */
int withdraw(unsigned int accountNum, unsigned int branchNum, double amount) {
    char *memo = malloc(sizeof(char) * MAX_MEMO_LENGTH + 1);
    checkNull(memo);
    sprintf(memo, "Withdrawed $%.2lf at the branch %x", amount, branchNum);
    memo[MAX_MEMO_LENGTH] = '\0';

    int result = makeTransaction(accountNum, branchNum, amount, memo);
    return result;
}

/**
 * Returns 0 if successful, -1 otherwise.
 * */
int transact(unsigned int from, unsigned int to, double amount) {
    char *memo = malloc(sizeof(char) * MAX_MEMO_LENGTH + 1);
    checkNull(memo);
    sprintf(memo, "Sent $%.2lf to %x", amount, to);
    memo[MAX_MEMO_LENGTH] = '\0';

    int result = makeTransaction(from, to, amount, memo);
    return result;
}

/**
 * Returns 0 if successful, -1 otherwise.
 * */
int makeTransaction(unsigned int from, unsigned int to, double amount, char *memo) {
    Account *sender = findAccount(from);
    Account *receiver = findAccount(to);

    int result = send(sender, amount);
    if (result == -1) {
        return -1;
    }
    receive(receiver, amount);

    createTransaction(sender, receiver, amount, memo);

    return 0;
}

/**
 * Returns 0 if successful, -1 otherwise.
 * */
int send(Account *sender, double amount) {
    pthread_mutex_lock(&(sender->lock));
    if (sender->amount >= amount) {
        sender->amount -= amount;
    } else {
        pthread_mutex_unlock(&(sender->lock));
        return -1;
    }
    pthread_mutex_unlock(&(sender->lock));

    return 0;
}

/**
 * Always successful.
 * */
int receive(Account *receiver, double amount) {
    pthread_mutex_lock(&(receiver->lock));
    receiver->amount += amount;
    pthread_mutex_unlock(&(receiver->lock));

    return 0;
}

int isCorrectPassword(unsigned int accountNum, char *password) {
    Account *account = findAccount(accountNum);
    if (strcmp(account->password, password) == 0) {
        return 1;
    }
    return 0;
}

Transaction *getTransactions(unsigned int accountNum) {
    Account *account = findAccount(accountNum);
    return account->lastTransaction;
}