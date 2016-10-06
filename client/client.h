#ifndef CLIENT_H
#define CLIENT_H

#include "../server/common.h"

int readcmd(char *buf, int size, struct command* ptrcmd);

#endif
