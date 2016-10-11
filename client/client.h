#ifndef CLIENT_H
#define CLIENT_H

#include "common.h"

int connectAddress(char *hostname, char *servname);

int recvReply(int connfd);

int readCommand(char *buf, int size);

#endif
