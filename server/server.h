#ifndef SERVER_H
#define SERVER_H

#include "common.h"
#include "command.h"

// create a listen socketfd
int createSocket(int port);

// accept socket connection from client
int acceptSocket(int sockfd);

// **********************
int connectSocket(int port, char *host);

// return 0 if success, otherwise connfd to be deleted in fdlist
int recvCommand(int connfd, struct Command *ptrcmd);

// send reply with code
int response(int sockfd, int rc, const char *reply);

#endif
