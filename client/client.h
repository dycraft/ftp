#ifndef CLIENT_H
#define CLIENT_H

#include "common.h"

#define MODE_PORT 1234
#define MODE_PASV 5678

int connectSocket(char *host, int port);
int acceptSocket(int sockfd);
int connectAddress(char *hostname, char *servname);

int recvReply(char *buffer, int connfd);
void showReply(int connfd);

int recvFile(int datafd, char *filename);
int sendFile(int datafd, char *filename);

int readCommand(char *buf, int size);

#endif
