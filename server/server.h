#ifndef SERVER_H
#define SERVER_H

#include "common.h"

// create a listen socketfd
int createSocket(int port);

// accept socket connection from client
int acceptSocket(int sockfd);

// **********************
int connectSocket(int port, char *host);

//
int recvCommand(int connfd, struct Command *ptrcmd);

// mutithread to execute command
void *p_executeCommand(void *arg);

#endif
