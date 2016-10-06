#ifndef COMMON_H
#define COMMON_H

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>
// #include <sys/epoll.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <unistd.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <memory.h>
#include <fcntl.h>
#include <pthread.h>

#define true 1
#define false 0
#define SUCC 0
#define FAIL -1

#define BUFFER_SIZE 8192
#define DEFAULT_PORT 14543

struct command {
  char name[5];
  char arg[256];
};

int createSocket(int port);
int acceptSocket(int sockfd);
int connectSocket(int port, char *host);


#endif
