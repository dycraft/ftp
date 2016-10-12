#include "command.h"

/* extern global variables init */

char *cmdlist[] = {
  "USER",
  "PASS",
  "SYST",
  "TYPE",
  "QUIT"
};

int (*execlist[])() = {
  &cmd_user,
  &cmd_pass,
  &cmd_syst,
  &cmd_type,
  &cmd_quit
};

/* command's methods */

void command_init(struct Command * cmd) {
  memset(cmd, 0, sizeof(*cmd));
}

void command_parse(struct Command * cmd, char *buf) {
  // protect the buffer string
  char s[BUFFER_SIZE];
  memset(s, 0, BUFFER_SIZE);
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

// check => false:Syntax error
int checkArg(int argc, int c, char format[]) {
  if (argc != c) {
    return false;
  } else {
    return true;
  }
}

/* cmd_functions */

int response(int sockfd, int rc, const char *reply) {
  char buf[BUFFER_SIZE];
  memset(buf, 0, BUFFER_SIZE);
  sprintf(buf, "%d %s\r\n", rc, reply);

  if (send(sockfd, buf, BUFFER_SIZE, 0) == -1) {
    printf("Error send(%d) to fd(%d): %s(%d)\n", rc, sockfd, strerror(errno), errno);
    return FAIL;
  } else {
    printf("Send to fd(%d): %s", sockfd, buf);
    return SUCC;
  }
}

// USER
int cmd_user(int argc, char *argv[], int connfd) {
  if (!checkArg(argc, 1, "USER [username]")) {
    response(connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'USER [username]'.");
    return FAIL;
  }

  if (strcmp(argv[0], "anonymous") != 0) {
    char buf[BUFFER_SIZE];
    memset(buf, 0, BUFFER_SIZE);
    sprintf(buf, "Username error, user:%s has no permission.", argv[0]);
    response(connfd, RC_ARG_ERR, buf);
    return FAIL;
  }

  // username pass
  response(connfd, RC_NEED_PASS, "Use 'PASS' command to input password.");

  return SUCC;
}

// PASS
int cmd_pass(int argc, char *argv[], int connfd) {
  if (!checkArg(argc, 1, "PASS [email_address]")) {
    response(connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'PASS [email_address]'.");
    return FAIL;
  }

  // login successfully
  response(connfd, RC_LOGIN, "Login successfully, welcome.");

  return SUCC;
}

// SYST
int cmd_syst(int argc, char *argv[], int connfd) {
  if (!checkArg(argc, 0, "SYST")) {
    response(connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'SYST'.");
    return FAIL;
  }

  response(connfd, RC_SYST, "UNIX Type: L8.");

  return SUCC;
}

// TYPE
int cmd_type(int argc, char *argv[], int connfd) {
  if (!checkArg(argc, 1, "TYPE [type]")) {
    response(connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'TYPE [type_num]'.");
    return FAIL;
  }

  if (strcmp(argv[0], "I") != 0) {
    char buf[BUFFER_SIZE];
    memset(buf, 0, BUFFER_SIZE);
    sprintf(buf, "Type error, not found type: %s.", argv[0]);
    response(connfd, RC_ARG_ERR, buf);
    return FAIL;
  }

  // username pass
  response(connfd, RC_CMD_OK, "Type set to I.");

  return SUCC;
}

int cmd_quit(int argc, char *argv[], int connfd) {
  if (!checkArg(argc, 0, "QUIT")) {
    response(connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'QUIT'.");
    return FAIL;
  }

  response(connfd, RC_LOGOUT, "User log out and quit the client.");

  return SUCC;
}
