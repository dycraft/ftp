#ifndef COMMAND_H
#define COMMAND_H

#include "common.h"

#define CMD_USER  0
#define CMD_PASS  1
#define CMD_PORT  2
#define CMD_PASV  3
#define CMD_RETR  4
#define CMD_SYST  5
#define CMD_TYPE  6
#define CMD_QUIT  7
#define CMD_ABOR  8

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

int cmd_user(int argc, char *argv[]);
int cmd_pass(int argc, char *argv[]);

extern void *execlist[] = {
  &cmd_user,
  &cmd_pass
}

void checkArg(int argc, char format[]);

#endif
