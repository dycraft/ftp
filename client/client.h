#ifndef CLIENT_H
#define CLIENT_H

#include "common.h"

// connect server with host address and port
int connectSocket(char *host, int port);
// accept the data translation socket from server in PORT mode.
int acceptSocket(int sockfd);
// connect server with address with no port(a process to try port)
int connectAddress(char *servname);

// get input in command line behind "ftp> "
int readCommand(char *buf, int size);
// recieve reply from server
int recvReply(char *buffer, int connfd);
// wrapper of recvreply()
void printReply(int connfd);

int recvFile(int datafd, char *filename);
int sendFile(int datafd, char *filename);



#endif
