#include "command.h"

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
    response(connfd, RC_ARG_ERR, "Username error, no permission.");
    return FAIL;
  }

  // username pass
  response(connfd, RC_NEED_PASS, "Use 'PASS' command to input password.");

  return SUCC;
}

// PASS
int cmd_pass(int argc, char *argv[], int connfd) {
  if (!checkArg(argc, 1, "PASS [email_address]")) {
    response(connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'PASS [email_address]'");
    return FAIL;
  }

  // login successfully
  response(connfd, RC_LOGIN, "Login successfully, welcome.");

  return SUCC;
}
