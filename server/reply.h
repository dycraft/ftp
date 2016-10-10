#ifndef REPLY_H
#define REPLY_H

#include "common.h"

#define RC_CMD_OK 200
#define RC_SYNTAX_ERR 501
#define RC_NO_IMP 502
#define RC_ARG_ERR  504

#define RC_NEW_USER 220
#define RC_NEED_PASS  331
#define RC_PASS_ERR 530
#define RC_LOGIN  230



extern char *reply[554];

void reply_init();

// send(rc) + htol()
int response(int sockfd, int rc);

#endif
