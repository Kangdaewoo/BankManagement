#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "state.h"
#include "server.h"


/**
 * Based on the sample code from http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html.
 * */

typedef struct customer {
    int sockFd;
    int outputFd;
    struct customer *next;
} Customer;


Customer *customers;

Customer *findCustomer(int sockFd) {
    Customer *customer = customers;
    while (customer->sockFd != sockFd) {
        customer = customer->next;
    }
    return customer;
}

void forwardToStateMachine(int sockFd) {
    Customer *customer = findCustomer(sockFd);

    char buf[MAX_BUF_LENGTH];
    int numRead = read(sockFd, buf, MAX_BUF_LENGTH - 1);
    buf[numRead] = '\0';
    printf("%s\n", buf);

    write(customer->outputFd, buf, strlen(buf));
}

void startStateMachine(Customer **customer) {
    int fd[2];
    pipe(fd);
    (*customer)->outputFd = fd[1];

    int argumentFd[2];
    argumentFd[0] = fd[0];
    argumentFd[1] = (*customer)->sockFd;

    pthread_t thread;
    int result = pthread_create(&thread, NULL, stateMachine, &argumentFd);
    if (result != 0) {
        perror("thread");
        exit(1);
    }
}

void addCustomer(int sockFd) {
    Customer *newCustomer = malloc(sizeof(Customer));
    checkNull(newCustomer);

    newCustomer->sockFd = sockFd;
    newCustomer->next = customers;
    customers = newCustomer;

    startStateMachine(&newCustomer);
}

void startServer() {
    int sockFd, newSockFd;
    socklen_t client;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;

    sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd < 0) {
        perror("socket");
        exit(1);
    }

    bzero((char *) &serverAddr, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(sockFd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind");
        exit(1);
    }

    listen(sockFd, 5);

    client = sizeof(clientAddr);

    customers = NULL;

    fd_set fds, copiedFds;
    FD_ZERO(&fds);
    FD_SET(sockFd, &fds);
    int maxFd = sockFd;
    while (1) {
        copiedFds = fds;
        if (select(maxFd + 1, &copiedFds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(1);
        }
        
        for (int i = 0; i <= maxFd; i++) {
            if (FD_ISSET(i, &copiedFds)) {
                if (i == sockFd) {
                    newSockFd = accept(sockFd, (struct sockaddr *) &clientAddr, &client);
                    if (newSockFd < 0) {
                        perror("accept");
                        exit(1);
                    }

                    FD_SET(newSockFd, &fds);
                    if (newSockFd > maxFd) {
                        maxFd = newSockFd;
                    }

                    addCustomer(newSockFd);
                } else {
                    forwardToStateMachine(i);
                }
            }
        }
    }
}