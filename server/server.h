#ifndef SERVER_H
#define SERVER_H

#include "common.h"
#include "command.h"

// check command line arguments
int handleCliArg(int argc, char *argv[]);

// create a listen socketfd
int createSocket(int port);

// accept socket connection from client
int acceptSocket(int sockfd);

// **********************
int connectSocket(int port, char *host);

// return 0 if success, otherwise connfd to be deleted in fdlist
int recvCommand(int connfd, struct Command *ptrcmd);

// mutithread to execute command
void *p_executeCommand(void *arg);

#endif
