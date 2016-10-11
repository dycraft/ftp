#include "command.h"
#include "reply.h"

/* extern global variables init */

char *cmdlist[] = {
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

int (*execlist[])() = {
  &cmd_user,
  &cmd_pass
};

/* command's methods */

void command_init(struct Command * cmd) {
  memset(cmd, 0, sizeof(*cmd));
}

void command_parse(struct Command * cmd, char *buf) {
  // protect the buffer string
  char s[BUFFER_SIZE];
  strcpy(s, buf);

  // strtok
  char *delim = " ";
  char *p;
  int i = 0;
  cmd->name = strtok(s, delim);
  while ((p = strtok(NULL, delim))) {
    cmd->argv[i] = p;
    i++;
  }
  cmd->argc = i;
}



/* common function in cmd_function */

// check => false:response(RC_SYNTAX_ERR_error)
//          true :response(rc_cmd_ok)
int checkArg(int argc, int c, char format[]) {
  if (argc != c) {
    printf("Syntax error. Input as\n\t%s", format);
    return false;
  } else {
    return true;
  }
}

/* cmd_functions */

// USER
int cmd_user(int argc, char *argv[], int connfd) {
  if (!checkArg(argc, 1, "USER [username]")) {
    response(connfd, RC_SYNTAX_ERR);
    return FAIL;
  }

  if (strcmp(argv[0], "anonymous") == 0) {
    response(connfd, RC_ARG_ERR);
    return FAIL;
  }

  response(connfd, RC_CMD_OK);

  // username pass
  response(connfd, RC_NEED_PASS);

  return SUCC;
}

// PASS
int cmd_pass(int argc, char *argv[], int connfd) {
  if (!checkArg(argc, 1, "PASS [email_address]")) {
    response(connfd, RC_SYNTAX_ERR);
    return FAIL;
  }

  response(connfd, RC_CMD_OK);

  // login successfully
  response(connfd, RC_LOGIN);

  return SUCC;
}
