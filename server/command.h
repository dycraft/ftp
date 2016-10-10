#ifndef COMMAND_H
#define COMMAND_H

#include "common.h"
#include "reply.h"

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


struct Command {
  char name[5];
  char arg[256];
};

// cmdlist: cmd_name array
extern char *cmdlist[] = {
  "USER",
  "PASS",
  "PORT",
  "PASV",
  "RETR",
  "STOR",
  "SYST",
  "TYPE",
  "QUIT",
  "ABOR"
};

//// cmd_functions

int cmd_user(int argc, char *argv[], int connfd);
int cmd_pass(int argc, char *argv[], int connfd);

//// common function in cmd_function

// check the amount of command's arguments
void checkArg(int argc, char format[]);

// execlist: function array
extern void *execlist[] = {
  &cmd_user,
  &cmd_pass
}

#endif
