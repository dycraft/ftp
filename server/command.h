#ifndef COMMAND_H
#define COMMAND_H

#include "common.h"
#include "fdlist.h"

// index of command
#define CMD_USER  0
#define CMD_PASS  1
#define CMD_SYST  2
#define CMD_TYPE  3
#define CMD_QUIT  4
#define CMD_PORT  5
#define CMD_PASV  6

#define CMD_NUM   7


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

int cmd_user(char *arg, struct Socketfd *fd);
int cmd_pass(char *arg, struct Socketfd *fd);
int cmd_syst(char *arg, struct Socketfd *fd);
int cmd_type(char *arg, struct Socketfd *fd);
int cmd_quit(char *arg, struct Socketfd *fd);
int cmd_port(char *arg, struct Socketfd *fd);
int cmd_pasv(char *arg, struct Socketfd *fd);


/* common function in cmd_function */

int randPort(int seed);

int decodeAddress(char *addr, int *port, char *buf);
int encodeAddress(char *buf, char *addr, int port);

#endif
