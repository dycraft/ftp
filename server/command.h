#ifndef COMMAND_H
#define COMMAND_H

#include "common.h"

// index of command
#define CMD_USER  0
#define CMD_PASS  1
#define CMD_PORT  2
#define CMD_PASV  3
#define CMD_RETR  4
#define CMD_SYST  5
#define CMD_TYPE  6
#define CMD_QUIT  7
#define CMD_ABOR  8

#define CMD_NUM   9


// struct Command and methods
#define ARG_MAX 3

struct Command {
  char *name;
  int argc;
  char *argv[ARG_MAX];
};

void command_init(struct Command *cmd);
void command_parse(struct Command *cmd, char *buf);


// cmdlist: cmd_name array
extern char *cmdlist[];
// execlist: function array
extern int (*execlist[])();

/* cmd_functions */

int cmd_user(int argc, char *argv[], int connfd);
int cmd_pass(int argc, char *argv[], int connfd);
int cmd_syst(int argc, char *argv[], int connfd);
int cmd_type(int argc, char *argv[], int connfd);
int cmd_quit(int argc, char *argv[], int connfd);


int response(int sockfd, int rc, const char *reply);

/* common function in cmd_function */

// check the amount of command's arguments
int checkArg(int argc, int c, char format[]);

#endif
