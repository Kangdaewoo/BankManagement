#include <pthread.h>


#if !defined(MAX_NAME_LENGTH) || !defined(MAX_MEMO_LENGTH) || !defined(MAX_BRANCH_NUM)
#define MAX_NAME_LENGTH 32 
#define MAX_MEMO_LENGTH 80
#define MAX_BRANCH_NUM 999
#endif


typedef struct transaction {
    unsigned int from;
    unsigned int to;
    double amount;
    char *memo;
    struct transaction *senderParent;
    struct transaction *receiverParent;
} Transaction;

typedef struct account {
    unsigned int number;
    char name[MAX_NAME_LENGTH + 1];
    char password[MAX_NAME_LENGTH + 1]; 
    double amount;
    struct transaction *lastTransaction;
    pthread_mutex_t lock;
} Account;


void initializeBank();

int isValidAccountNumber(unsigned int accountNum);
int isCorrectPassword(unsigned int accountNum, char *password);

Account *findAccount(unsigned int accountNum);
/**
 * Account info in string.
 * Must be freed after used.
 * */
char *getAccountInfo(unsigned int accountNum);
Transaction *getTransactions(unsigned int accountNum);

unsigned int createAccount(char *name, char *password, int isBranch);
Transaction *createTransaction(Account *sender, Account *receiver, double amount, char *memo);

/**
 * Returns 0 if successful, -1 otherwise.
 * */
int deposit(unsigned int accountNum, unsigned int branchNum, double amount);
int withdraw(unsigned int accountNum, unsigned int branchNum, double amount);
int transact(unsigned int from, unsigned int to, double amount);