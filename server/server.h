#ifndef SERVER_H
#define SERVER_H

#include "common.h"

int createSocket(int port);
int acceptSocket(int sockfd);
int connectSocket(int port, char *host);
void handleClient(int connfd);

int connectDataSocket(int connfd);

//int recvData(int sockfd, char *buf, int size);
//int sendData(int sockfd, char *buf, int size);
int response(int sockfd, int rc);
int recvCommand(int connfd, (struct Command *) ptrcmd);

#endif
