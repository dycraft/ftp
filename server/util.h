#ifndef UTIL_H
#define UTIL_H

#include "common.h"

// get server's ip
int getip(char *ip);

/* common function in cmd_function */

int randPort(int seed);

int decodeAddress(char *addr, int *port, char *buf);
int encodeAddress(char *buf, char *addr, int port);

int isDir(char *path);

#endif
