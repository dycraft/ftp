#include "command.h"
#include "server.h"

/* extern global variables init */

char *cmdlist[] = {
  "USER",
  "PASS",
  "SYST",
  "TYPE",
  "QUIT",
  "PORT",
  "PASV",
  "LIST",
  "RETR",
  "STOR"
};

int (*execlist[])() = {
  &cmd_user,
  &cmd_pass,
  &cmd_syst,
  &cmd_type,
  &cmd_quit,
  &cmd_port,
  &cmd_pasv,
  &cmd_list,
  &cmd_retr,
  &cmd_stor
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

// QUIT
int cmd_quit(char *arg, struct Socketfd *fd) {
  if (strlen(arg)) {
    response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'QUIT'.");
    return FAIL;
  }

  response(fd->connfd, RC_LOGOUT, "User log out and quit the client.");

  return SUCC;
}

// PORT
int cmd_port(char *arg, struct Socketfd *fd) {
  char address[16];
  int port;
  if (!strlen(arg)) {
    response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'PORT h1,h2,h3,h4,p1,p2'");
    return FAIL;
  } else {
    if (decodeAddress(address, &port, arg) == FAIL) {
      response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'PORT h1,h2,h3,h4,p1,p2', p1,p2 = (0~255).");
      return FAIL;
    }
  }

  // restore the sockaddr_in
  fd->addr.sin_family = AF_INET;
  fd->addr.sin_port = htons(port);
  inet_pton(AF_INET, address, &fd->addr.sin_addr);

  fd->mode = MODE_PORT;
  fd->transfd = 0;

  char buf[BUFFER_SIZE];
  memset(buf, 0, BUFFER_SIZE);
  sprintf(buf, "Entering Port Mode. (%s)", arg);
  response(fd->connfd, RC_CMD_OK, buf);

  return SUCC;
}

// PASV
int cmd_pasv(char *arg, struct Socketfd *fd) {
  if (strlen(arg)) {
    response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'PASV'.");
    return FAIL;
  }

  int port = randPort(fd->connfd);
  fd->transfd = createSocket(port);

  char buf[BUFFER_SIZE];
  memset(buf, 0, BUFFER_SIZE);
  char ip[16];
  if (getip(ip) == FAIL) {
    printf("Error *getip().");
  }
  if (encodeAddress(buf, ip, port) == FAIL) {
    printf("Error *encodeAddress().\n");
    response(fd->connfd, RC_EXEC_ERR, "Command execute error, input again.");
    return FAIL;
  }

  fd->mode = MODE_PASV;
  memset(&(fd->addr), 0, sizeof(struct sockaddr_in));

  char buffer[BUFFER_SIZE];
  memset(buffer, 0, BUFFER_SIZE);
  sprintf(buffer, "Entering Passive Mode. (%s)", buf);
  response(fd->connfd, RC_CMD_OK, buffer);

  return SUCC;
}

int cmd_list(char *arg, struct Socketfd *fd) {
  if (strlen(arg)) {
    response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'LIST'.");
    return FAIL;
  }

  // put info into file
  if (system("ls -l | tail -n+2 > .ls") < 0) {
    printf("Error system(ls).\n");
    return FAIL;
  }

  // fopen
  FILE *file = fopen(".ls", "r");
  if (!file) {
    printf("Error fopen('.ls').\n");
		return FAIL;
	}

  char buf[DATA_SIZE];
  memset(buf, 0, DATA_SIZE);
  if (fread(buf, DATA_SIZE-1, DATA_ITEM, file) == FAIL) {
    printf("Error fread().\n");
    return FAIL;
  }

  response(fd->connfd, RC_CMD_OK, buf);

  fclose(file);

  if (system("rm .ls") < 0) {
    printf("Error system(rm).\n");
    return FAIL;
  }

  return SUCC;
}

// RETR
int cmd_retr(char *arg, struct Socketfd *fd) {
  if (!strlen(arg)) {
    response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'RETR [filename]'.");
    return FAIL;
  }

  // create data connection
  int datafd = createDataSocket(fd);
  if (datafd == FAIL) {
    response(fd->connfd, RC_NO_CNN, "No TCP connection was established.");
    return FAIL;
  }

  // send file
  char buf[BUFFER_SIZE];
  memset(buf, 0, BUFFER_SIZE);
  strcpy(buf, root);
  strcat(buf, arg);
  if (sendFile(datafd, fd->connfd, buf) == FAIL) {
    printf("Error *sendFile(%d, %s).", datafd, buf);
  }

  close(datafd);

  // init data connection
  memset(&(fd->addr), 0, sizeof(fd->addr));
  if (fd->transfd > 0) {
    fd->transfd = 0;
  }

  return SUCC;
}

int cmd_stor(char *arg, struct Socketfd *fd) {
  if (!strlen(arg)) {
    response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'STOR [filename]'.");
    return FAIL;
  }

  // create data connection
  int datafd = createDataSocket(fd);
  if (datafd == FAIL) {
    response(fd->connfd, RC_NO_CNN, "No TCP connection was established.");
    return FAIL;
  }

  // recv file
  char buf[BUFFER_SIZE];
  memset(buf, 0, BUFFER_SIZE);
  strcpy(buf, root);
  strcat(buf, arg);
  if (recvFile(datafd, fd->connfd, buf) == FAIL) {
    printf("Error *recvFile(%d, %s).", datafd, buf);
  }

  close(datafd);

  // init data connection
  memset(&(fd->addr), 0, sizeof(fd->addr));
  if (fd->transfd > 0) {
    fd->transfd = 0;
  }

  return SUCC;
}

/* common function in cmd_function */

int randPort(int seed) {
  srand(seed);
  return rand() % (65536 - 20000) + 20000;
}

int decodeAddress(char *addr, int *port, char *buf) {
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

int encodeAddress(char *buf, char *addr, int port) {
  int h1, h2, h3, h4, p1, p2;
  int num = sscanf(addr, "%d.%d.%d.%d", &h1, &h2, &h3, &h4);
  if (num != 4) {
    return FAIL;
  }

  p1 = port / 256;
  p2 = port % 256;

  sprintf(buf, "%d,%d,%d,%d,%d,%d", h1, h2, h3, h4, p1, p2);

  return SUCC;
}
