#ifndef COMMAND_H
#define COMMAND_H

#include "common.h"


#define HANDLE_NUM 6

// struct Command and methods
#define NAME_LEN  5
#define ARG_LEN 32

struct Command {
  char name[NAME_LEN];
  char arg[ARG_LEN];
};

// handlelist: array of cmd_name to be handled
extern char *handlelist[];
// handle: function array
extern int (*handler[])();


int command_parse(struct Command *cmd, char *buf);

int handle_quit(char *arg, struct Status *status);
int handle_port(char *arg, struct Status *status);
int handle_pasv(char *arg, struct Status *status);
int handle_retr(char *arg, struct Status *status);
int handle_stor(char *arg, struct Status *status);
int handle_list(char *arg, struct Status *status);

#endif
