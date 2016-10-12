#ifndef CLIENT_H
#define CLIENT_H

#include "common.h"

int connectSocket(char *host, int port);
int connectAddress(char *hostname, char *servname);

int recvReply(int connfd);
void showReply(int connfd);

int readCommand(char *buf, int size);

#endif
