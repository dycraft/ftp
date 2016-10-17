#ifndef COMMAND_H
#define COMMAND_H

#include "common.h"

// index of command
#define CMD_USER  0
#define CMD_PASS  1
#define CMD_SYST  2
#define CMD_TYPE  3
#define CMD_QUIT  4

#define CMD_NUM   5


// struct Command and methods
#define NAME_LEN  5
#define ARG_LEN 32

struct Command {
  char name[NAME_LEN];
  char arg[ARG_LEN];
};

void command_parse(struct Command *cmd, char *buf);


// cmdlist: cmd_name array
extern char *cmdlist[];
// execlist: function array
extern int (*execlist[])();

/* cmd_functions */

int cmd_user(char *arg, int connfd);
int cmd_pass(char *arg, int connfd);
int cmd_syst(char *arg, int connfd);
int cmd_type(char *arg, int connfd);
int cmd_quit(char *arg, int connfd);
int cmd_port(char *arg, int connfd);
int cmd_pasv(char *arg, int connfd);


/* common function in cmd_function */


#endif
