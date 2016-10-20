#ifndef COMMON_H
#define COMMON_H

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
// #include <sys/epoll.h>
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
#include <pthread.h>

#ifndef max
#define max(a,b)  (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif

#define true 1
#define false 0
#define SUCC 0
#define FAIL -1

#define BUFFER_SIZE 512
#define DATA_SIZE 1024

#define DEFAULT_PORT  21
#define DEFAULT_ROOT  "tmp/"

extern int port;
extern char *root;

/* cmd rc */
#define RC_CMD_OK 200
#define RC_SYNTAX_ERR 501
#define RC_NO_IMP 502
#define RC_ARG_ERR  504
#define RC_EXEC_ERR 550

#define RC_NEW_USER 220
#define RC_NEED_PASS  331
#define RC_PASS_ERR 530
#define RC_LOGIN  230
#define RC_LOGOUT 221
#define RC_PASV_OK 227

#define RC_NO_FILE  550
#define RC_FILE_OK  150
#define RC_NO_CNN 425
#define RC_NET_ERR 426
#define RC_LOC_ERR  451
#define RC_TRANS_OK 226

#define RC_SYST 215
/* --- --- */

#endif
