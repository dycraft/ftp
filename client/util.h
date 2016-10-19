#ifndef UTIL_H
#define UTIL_H

#include "common.h"

int randPort(int seed);

int getip(char *ip);

int decodeAddress(char *addr, int *port, char *buf);
int encodeAddress(char *buf, char *addr, int port);

#endif
