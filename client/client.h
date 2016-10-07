#ifndef CLIENT_H
#define CLIENT_H

#include "../server/common.h"

int readCommand(char *buf, int size, struct Command* ptrcmd);

#endif
