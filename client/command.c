#include "command.h"
#include "client.h"

/* extern global variables init */

char *handlelist[] = {
  "QUIT",
  "PORT",
  "PASV",
  "RETR",
  "STOR"
};

int (*handle[])() = {
  &handle_quit,
  &handle_port,
  &handle_pasv,
  &handle_retr,
  &handle_stor
};

/* command's methods */

int command_parse(struct Command * cmd, char *buf) {
  memset(cmd, 0, sizeof(struct Command));
  sscanf(buf, "%s %s", cmd->name, cmd->arg);
  if ((strlen(cmd->name) >= NAME_LEN) || (strlen(cmd->arg) >= ARG_LEN)) {
    return FAIL;
  } else {
    return SUCC;
  }
}


int handle_quit(char *reply, struct Status *status) {
  close(status->connfd);
  if (status->datafd > 0) {
    close(status->datafd);
  }
  exit(0);
}

int handle_port(char *reply, struct Status *status) {

}

int handle_pasv(char *reply, struct Status *status) {

}

int handle_retr(char *reply, struct Status *status) {

}

int handle_stor(char *reply, struct Status *status) {

}
