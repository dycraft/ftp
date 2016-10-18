#ifndef SERVER_H
#define SERVER_H

#include "common.h"
#include "command.h"

// create a listen socketfd
int createSocket(int port);
// accept socket connection from client
int acceptSocket(int sockfd);

// data connection
int createDataSocket(struct Socketfd *fd);
int createPortSocket(struct sockaddr_in *addr);
int createPasvSocket(int listenfd);

// data translation
int sendFile(int datafd, int connfd, char *filename);
int recvFile(int datafd, int connfd, char *filename);

// return 0 if success, otherwise connfd to be deleted in fdlist
int recvCommand(int connfd, struct Command *ptrcmd);
// send reply with code
int response(int sockfd, int rc, const char *reply);

// get server's ip
int getip(char *ip);

#endif
