#include "command.h"
#include "server.h"

/* extern global variables init */

char *cmdlist[] = {
  "USER",
  "PASS",
  "SYST",
  "TYPE",
  "QUIT",
  "PORT"
};

int (*execlist[])() = {
  &cmd_user,
  &cmd_pass,
  &cmd_syst,
  &cmd_type,
  &cmd_quit,
  &cmd_port
};

/* command's methods */

void command_parse(struct Command * cmd, char *buf) {
  // strtok
  char *delim = " ";
  char *p;
  if ((p = strtok(buf, delim))) {
    strcpy(cmd->name, p);
  }
  if ((p = strtok(NULL, delim))) {
    strcpy(cmd->arg, p);
  }
}


/* common function in cmd_function */



/* cmd_functions */

// USER
int cmd_user(char *arg, struct Socketfd *fd) {
  if (!strlen(arg)) {
    response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'USER [username]'.");
    return FAIL;
  }

  if (strcmp(arg, "anonymous") != 0) {
    char buf[BUFFER_SIZE];
    memset(buf, 0, BUFFER_SIZE);
    sprintf(buf, "Username error, user:%s has no permission.", arg);
    response(fd->connfd, RC_ARG_ERR, buf);
    return FAIL;
  }

  // username pass
  response(fd->connfd, RC_NEED_PASS, "Use 'PASS' command to input password.");

  return SUCC;
}

// PASS
int cmd_pass(char *arg, struct Socketfd *fd) {
  if (!strlen(arg)) {
    response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'PASS [email_address]'.");
    return FAIL;
  }

  // login successfully
  response(fd->connfd, RC_LOGIN, "Login successfully, welcome.");

  return SUCC;
}

// SYST
int cmd_syst(char *arg, struct Socketfd *fd) {
  if (strlen(arg)) {
    response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'SYST'.");
    return FAIL;
  }

  response(fd->connfd, RC_SYST, "UNIX Type: L8.");

  return SUCC;
}

// TYPE
int cmd_type(char *arg, struct Socketfd *fd) {
  if (!strlen(arg)) {
    response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'TYPE [type_num]'.");
    return FAIL;
  }

  if (strcmp(arg, "I") != 0) {
    char buf[BUFFER_SIZE];
    memset(buf, 0, BUFFER_SIZE);
    sprintf(buf, "Type error, not found type: %s.", arg);
    response(fd->connfd, RC_ARG_ERR, buf);
    return FAIL;
  }

  // username pass
  response(fd->connfd, RC_CMD_OK, "Type set to I.");

  return SUCC;
}

int cmd_quit(char *arg, struct Socketfd *fd) {
  if (strlen(arg)) {
    response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'QUIT'.");
    return FAIL;
  }

  response(fd->connfd, RC_LOGOUT, "User log out and quit the client.");

  return SUCC;
}

int cmd_port(char *arg, struct Socketfd *fd) {
  char address[16];
  int port;
  if (!strlen(arg)) {
    response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'PORT h1,h2,h3,h4,p1,p2'");
    return FAIL;
  } else {
    if (address_parse(address, &port, arg) == FAIL) {
      response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'PORT h1,h2,h3,h4,p1,p2', p1,p2 = (0~255).");
      return FAIL;
    }
  }

  // restore the sockaddr_in
  fd->addr.sin_family = AF_INET;
  fd->addr.sin_port = htons(port);
  inet_pton(AF_INET, address, &fd->addr.sin_addr);

  fd->mode = MODE_PORT;

  char buf[BUFFER_SIZE];
  sprintf(buf, "Convert to PORT mode successfully. (%s:%d)", address, port);
  response(fd->connfd, RC_CMD_OK, buf);

  return SUCC;
}

/* common function in cmd_function */

int address_parse(char *addr, int *port, char *buf) {
  int h1, h2, h3, h4, p1, p2;
  int num = sscanf(buf, "%d,%d,%d,%d,%d,%d", &h1, &h2, &h3, &h4, &p1, &p2);
  if (num != 6) {
    return FAIL;
  }

  // port
  if ((p1 >= 0) && (p2 >= 0) && (p1 < 256) && (p2 < 256)) {
    *port = p1 * 256 + p2;
  } else {
    return FAIL;
  }

  // addr
  sprintf(addr, "%d.%d.%d.%d", h1, h2, h3, h4);

  return SUCC;
}
