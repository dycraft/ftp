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
#define CMD_LIST  7
#define CMD_RETR  8
#define CMD_STOR  9
#define CMD_ABOR  10
#define CMD_CWD   11
#define CMD_CDUP  12
#define CMD_PWD   13
#define CMD_DELE  14
#define CMD_MKD   15
#define CMD_RMD   16
#define CMD_RNFR  17
#define CMD_RNTO  18

#define CMD_NUM   14


// struct Command and methods
#define NAME_LEN  5
#define ARG_LEN 32

struct Command {
  char name[NAME_LEN];
  char arg[ARG_LEN];
};

int command_parse(struct Command *cmd, char *buf);


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
int cmd_list(char *arg, struct Socketfd *fd);
int cmd_pasv(char *arg, struct Socketfd *fd);
int cmd_retr(char *arg, struct Socketfd *fd);
int cmd_stor(char *arg, struct Socketfd *fd);
int cmd_abor(char *arg, struct Socketfd *fd);
int cmd_cwd(char *arg, struct Socketfd *fd);
int cmd_cdup(char *arg, struct Socketfd *fd);
int cmd_pwd(char *arg, struct Socketfd *fd);

#endif
