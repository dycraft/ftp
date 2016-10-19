#ifndef COMMON_H
#define COMMON_H

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <ifaddrs.h>
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

#define BUFFER_SIZE 512
#define DATA_ITEM 1
#define DATA_SIZE 256

#define DEFAULT_PORT  21
#define DEFAULT_ROOT  "/tmp"

struct Status {
  int connfd;
  int datafd;
  int mode;
};

#define MODE_NORM 0
#define MODE_PASV 789
#define MODE_PORT 456

#endif
