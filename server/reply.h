#ifndef REPLY_H
#define REPLY_H

#include "common.h"

#define RC_CMD_OK 200
#define RC_NEW_USER 220
#define RC_LOGIN  230
#define RC_SYNTAX 501

extern char *reply[554];

void reply_init();

int response(int sockfd, int rc);

#endif
