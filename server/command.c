#include "command.h"
#include "server.h"

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
int cmd_user(char *arg, int connfd) {
  if (!strlen(arg)) {
    response(connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'USER [username]'.");
    return FAIL;
  }

  if (strcmp(arg, "anonymous") != 0) {
    char buf[BUFFER_SIZE];
    memset(buf, 0, BUFFER_SIZE);
    sprintf(buf, "Username error, user:%s has no permission.", arg);
    response(connfd, RC_ARG_ERR, buf);
    return FAIL;
  }

  // username pass
  response(connfd, RC_NEED_PASS, "Use 'PASS' command to input password.");

  return SUCC;
}

// PASS
int cmd_pass(char *arg, int connfd) {
  if (!strlen(arg)) {
    response(connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'PASS [email_address]'.");
    return FAIL;
  }

  // login successfully
  response(connfd, RC_LOGIN, "Login successfully, welcome.");

  return SUCC;
}

// SYST
int cmd_syst(char *arg, int connfd) {
  if (strlen(arg)) {
    response(connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'SYST'.");
    return FAIL;
  }

  response(connfd, RC_SYST, "UNIX Type: L8.");

  return SUCC;
}

// TYPE
int cmd_type(char *arg, int connfd) {
  if (!strlen(arg)) {
    response(connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'TYPE [type_num]'.");
    return FAIL;
  }

  if (strcmp(arg, "I") != 0) {
    char buf[BUFFER_SIZE];
    memset(buf, 0, BUFFER_SIZE);
    sprintf(buf, "Type error, not found type: %s.", arg);
    response(connfd, RC_ARG_ERR, buf);
    return FAIL;
  }

  // username pass
  response(connfd, RC_CMD_OK, "Type set to I.");

  return SUCC;
}

int cmd_quit(char *arg, int connfd) {
  if (strlen(arg)) {
    response(connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'QUIT'.");
    return FAIL;
  }

  response(connfd, RC_LOGOUT, "User log out and quit the client.");

  return SUCC;
}

int cmd_port(char *arg, int connfd) {
  char address[16];
  int port;
  if (!strlen(arg)) {
    if (address_parse(address, &port, arg) == FAIL) {
      response(connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'PORT h1,h2,h3,h4,p1,p2', p1,p2 = (0~255).");
      return FAIL;
    }
  }

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  inet_pton(AF_INET, address, &addr.sin_addr);

  response(connfd, RC_CMD_OK, "Convert to PORT mode successfully.");

  return SUCC;
}

/* common function in cmd_function */

int address_parse(char *addr, int *port, char *buf) {
  int len = strlen(buf);
  int count = 0;
  int pos = 0;
  for (int i = 0; i < len; i++) {
    if (buf[i] == ',') {
      count++;
      if (count == 4) {
        pos = i;
      }
    }
  }

  if (count != 5) {
    return FAIL;
  }

  // port
  char buffer[BUFFER_SIZE];
  char *delim = ",";
  memset(buffer, 0, BUFFER_SIZE);
  strcpy(buffer, addr + pos + 1);
  char *p1 = strtok(buffer, delim);
  char *p2 = strtok(NULL, delim);
  if (p1 && p2) {
    int tmp1 = atoi(p1), tmp2 = atoi(p2);
    if ((tmp1 >= 0) && (tmp2 >= 0) && (tmp1 < 256) && (tmp2 < 256)) {
      *port = atoi(p1) * 256 + atoi(p2);
    } else {
      return FAIL;
    }
  } else {
    return FAIL;
  }

  // address
  strncpy(addr, buf, pos);
  for (int i = 0; i < pos; i++) {
    if (addr[i] == ',') {
      addr[i] = '.';
    }
  }

  return SUCC;
}
