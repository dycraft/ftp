#ifndef COMMON_H
#define COMMON_H

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <memory.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#define true 1
#define false 0
#define SUCC 0
#define FAIL -1

#define BUFFER_SIZE 1024

#define DEFAULT_PORT  21
#define DEFAULT_ROOT  "/tmp"

extern int port;
extern char *root;

#endif
