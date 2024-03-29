#include "command.h"
#include "server.h"
#include "util.h"

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
  "STOR",
  "ABOR",
  "CWD",
  "CDUP",
  "PWD",
  "DELE",
  "MKD",
  "RMD",
  "RNFR",
  "RNTO"
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
  &cmd_stor,
  &cmd_abor,
  &cmd_cwd,
  &cmd_cdup,
  &cmd_pwd,
  &cmd_dele,
  &cmd_mkd,
  &cmd_rmd,
  &cmd_rnfr,
  &cmd_rnto
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

int check_mode(int connfd, int mode, int require) {

  if ((require & mode) == mode) {
    return true;
  } else if (require & RQ_LOGIN) {
    response(connfd, RC_NOT_LOG, "Login first.");
    return false;
  } else if (require & RQ_TRANS) {
    response(connfd, RC_EXEC_ERR, "Use PORT or PASV command first.");
    return false;
  } else if (require & RQ_RENM) {
    response(connfd, RC_EXEC_ERR, "Use RNFR command first.");
    return false;
  } else if (require & RQ_USER) {
    response(connfd, RC_NOT_LOG, "Use USER command first.");
    return false;
  }   else {
    return true;
  }
}

/* cmd_functions */

// USER
int cmd_user(char *arg, struct Socketfd *fd) {

  if (!strlen(arg)) {
    response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'USER [username]'.");
    return FAIL;
  }

  // check mode
  if (!check_mode(fd->connfd, fd->mode, RQ_NONE)) {
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

  fd->mode = MODE_USER;

  return SUCC;
}

// PASS
int cmd_pass(char *arg, struct Socketfd *fd) {

  // check mode
  if (!check_mode(fd->connfd, fd->mode, RQ_USER)) {
    return FAIL;
  }

  // login successfully
  response(fd->connfd, RC_LOGIN, "Login successfully, welcome.");

  fd->mode = MODE_LOGIN;

  return SUCC;
}

// SYST
int cmd_syst(char *arg, struct Socketfd *fd) {

  if (strlen(arg)) {
    response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'SYST'.");
    return FAIL;
  }

  // check mode
  if (!check_mode(fd->connfd, fd->mode, RQ_NONE)) {
    return FAIL;
  }

  response(fd->connfd, RC_SYST, "UNIX Type: L8");

  return SUCC;
}

// TYPE
int cmd_type(char *arg, struct Socketfd *fd) {

  if (!strlen(arg)) {
    response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'TYPE [type_num]'.");
    return FAIL;
  }

  // check mode
  if (!check_mode(fd->connfd, fd->mode, RQ_COMN)) {
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

  // check mode
  if (!check_mode(fd->connfd, fd->mode, RQ_NONE)) {
    return FAIL;
  }

  response(fd->connfd, RC_LOGOUT, "User log out and quit the client.");

  // handled by main function
  fd->mode = MODE_QUIT;

  return SUCC;
}

// ABOR
int cmd_abor(char *arg, struct Socketfd *fd) {

  if (strlen(arg)) {
    response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'ABOR'.");
    return FAIL;
  }

  // check mode
  if (!check_mode(fd->connfd, fd->mode, RQ_NONE)) {
    return FAIL;
  }

  response(fd->connfd, RC_LOGOUT, "User log out and quit the client.");

  // handled by main function
  fd->mode = MODE_QUIT;

  return SUCC;
}

// PORT
int cmd_port(char *arg, struct Socketfd *fd) {

  if (!strlen(arg)) {
    response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'PORT h1,h2,h3,h4,p1,p2'");
    return FAIL;
  }

  // check mode
  if (!check_mode(fd->connfd, fd->mode, RQ_COMN)) {
    return FAIL;
  }

   // check address
  char address[16];
  int port;
  if (decodeAddress(address, &port, arg) == FAIL) {
    response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'PORT h1,h2,h3,h4,p1,p2', p1,p2 = (0~255).");
    return FAIL;
  }

  // restore the sockaddr_in
  memset(&(fd->addr), 0, sizeof(struct sockaddr_in));
  fd->addr.sin_family = AF_INET;
  fd->addr.sin_port = htons(port);
  inet_pton(AF_INET, address, &fd->addr.sin_addr);

  // clear the pasv mode
  fd->mode = MODE_PORT;
  if (fd->transfd > 0) {
    close(fd->transfd);
    fd->transfd = 0;
  }

  char buf[BUFFER_SIZE];
  memset(buf, 0, BUFFER_SIZE);
  sprintf(buf, "Entering Port Mode. (%s)", arg);
  response(fd->connfd, RC_CMD_OK, buf);

  fd->mode = MODE_PORT;

  return SUCC;
}

// PASV
int cmd_pasv(char *arg, struct Socketfd *fd) {

  if (strlen(arg)) {
    response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'PASV'.");
    return FAIL;
  }

  // check mode
  if (!check_mode(fd->connfd, fd->mode, RQ_COMN)) {
    return FAIL;
  }

  // restor the listenfd (transfd)
  int port = randPort(fd->connfd);
  fd->transfd = createSocket(port);
  if (fd->transfd == FAIL) {
    printf("Error createSocket(%d): %s(%d)\n", port, strerror(errno), errno);
    response(fd->connfd, RC_LOC_ERR, "Server error: cannot create socket.");
    return FAIL;
  }

  char buf[BUFFER_SIZE];
  memset(buf, 0, BUFFER_SIZE);
  char ip[16];
  if (getip(ip) == FAIL) {
    printf("Error *getip().");
    response(fd->connfd, RC_LOC_ERR, "Server error: cannot get server's ip address.");
    return FAIL;
  }
  if (encodeAddress(buf, ip, port) == FAIL) {
    printf("Error *encodeAddress().\n");
    response(fd->connfd, RC_EXEC_ERR, "Command execute error, input again.");
    return FAIL;
  }

  // clear the port mode
  fd->mode = MODE_PASV;
  memset(&(fd->addr), 0, sizeof(struct sockaddr_in));

  char buffer[BUFFER_SIZE];
  memset(buffer, 0, BUFFER_SIZE);
  sprintf(buffer, "Entering Passive Mode (%s)", buf);
  response(fd->connfd, RC_PASV_OK, buffer);

  fd->mode = MODE_PASV;

  return SUCC;
}

int cmd_list(char *arg, struct Socketfd *fd) {

  if (strlen(arg)) {
    response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'LIST'.");
    return FAIL;
  }

  // check mode
  if (!check_mode(fd->connfd, fd->mode, RQ_TRANS | RQ_LOGIN)) {
    return FAIL;
  }

  // create data connection
  int datafd = createDataSocket(fd);
  if (datafd == FAIL) {
    response(fd->connfd, RC_NO_CNN, "No TCP connection was established.");
    return FAIL;
  }

  char b[BUFFER_SIZE];
  memset(b, 0, BUFFER_SIZE);
  sprintf(b, "Server is listing dir:%s ...", fd->dir);
  response(fd->connfd, RC_FILE_OK, b);

  int p = dup(1);
  dup2(datafd, 1);
  char sys[BUFFER_SIZE];
  memset(sys, 0, BUFFER_SIZE);
  sprintf(sys, "cd %s && ls -l", fd->dir);
  system(sys);
  dup2(p, 1);
  close(datafd);

  response(fd->connfd, RC_TRANS_OK, "LIST successfully.");

  // init data connection
  memset(&(fd->addr), 0, sizeof(fd->addr));
  if (fd->transfd > 0) {
    close(fd->transfd);
    fd->transfd = 0;
  }

  fd->mode = MODE_LOGIN;

  return SUCC;
}

// RETR
int cmd_retr(char *arg, struct Socketfd *fd) {

  if (!strlen(arg)) {
    response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'RETR [filename]'.");
    return FAIL;
  }

  // check mode
  if (!check_mode(fd->connfd, fd->mode, RQ_TRANS | RQ_LOGIN)) {
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
  sprintf(buf, "%s/%s", fd->dir, arg);
  if (sendFile(datafd, fd->connfd, buf) == FAIL) {
    printf("Error *sendFile(%d, %s).\n", datafd, buf);
  }

  close(datafd);

  // init data connection
  memset(&(fd->addr), 0, sizeof(fd->addr));
  if (fd->transfd > 0) {
    close(fd->transfd);
    fd->transfd = 0;
  }

  fd->mode = MODE_LOGIN;

  return SUCC;
}

int cmd_stor(char *arg, struct Socketfd *fd) {

  if (!strlen(arg)) {
    response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'STOR [filename]'.");
    return FAIL;
  }

  // check mode
  if (!check_mode(fd->connfd, fd->mode, RQ_TRANS | RQ_LOGIN)) {
    fd->mode = MODE_LOGIN;
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
  sprintf(buf, "%s/%s", fd->dir, arg);
  if (recvFile(datafd, fd->connfd, buf) == FAIL) {
    printf("Error *recvFile(%d, %s).", datafd, buf);
  }

  close(datafd);

  // init data connection
  memset(&(fd->addr), 0, sizeof(fd->addr));
  if (fd->transfd > 0) {
    close(fd->transfd);
    fd->transfd = 0;
  }

  fd->mode = MODE_LOGIN;

  return SUCC;
}

int cmd_cwd(char *arg, struct Socketfd *fd) {
  if (!strlen(arg)) {
    response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'CWD [dirname]'.");
    return FAIL;
  }

  // check mode
  if (!check_mode(fd->connfd, fd->mode, RQ_COMN)) {
    return FAIL;
  }

  if (arg[0] == '.' || arg[0] == '/') {
    response(fd->connfd, RC_ARG_ERR, "Command argument error, patterns begin with '.' and '/' are not permitted.");
    return FAIL;
  }

  char tmp[DIR_SIZE];
  memset(tmp, 0, DIR_SIZE);
  sprintf(tmp, "%s/%s", fd->dir, arg);

  if (!isDir(tmp)) {
    char buf[BUFFER_SIZE];
    memset(buf, 0, BUFFER_SIZE);
    sprintf(buf, "Command argument error, Dir:%s doesn't exist.", tmp);
    response(fd->connfd, RC_ARG_ERR, buf);
    return FAIL;
  }

  // clear the end '/'
  int end = strlen(fd->dir) - 1;
  if (fd->dir[end] == '/') {
    fd->dir[end] = '\0';
  }

  strcpy(fd->dir, tmp);

  char b[BUFFER_SIZE];
  memset(b, 0, BUFFER_SIZE);
  sprintf(b, "DIR: %s", fd->dir);
  response(fd->connfd, RC_CMD_OK, b);

  return SUCC;
}

int cmd_cdup(char *arg, struct Socketfd *fd) {
  if (strlen(arg)) {
    response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'CDUP'.");
    return FAIL;
  }

  // check mode
  if (!check_mode(fd->connfd, fd->mode, RQ_COMN)) {
    return FAIL;
  }

  if (strcmp(fd->dir, root) == 0) {
    response(fd->connfd, RC_EXEC_ERR, "Command not permitted, DIR is root dir: /tmp.");
    return FAIL;
  }

  // clear char from end util '/'
  int end = strlen(fd->dir) - 1;
  for (int i = end; i > 0; i--) {
    if (fd->dir[i] == '/') {
      fd->dir[i] = 0;
      break;
    } else {
      fd->dir[i] = 0;
    }
  }

  char b[BUFFER_SIZE];
  memset(b, 0, BUFFER_SIZE);
  sprintf(b, "DIR: %s", fd->dir);
  response(fd->connfd, RC_CMD_OK, b);

  return SUCC;
}

int cmd_pwd(char *arg, struct Socketfd *fd) {
  if (strlen(arg)) {
    response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'PWD'.");
    return FAIL;
  }

  // check mode
  if (!check_mode(fd->connfd, fd->mode, RQ_COMN)) {
    return FAIL;
  }

  char b[BUFFER_SIZE];
  memset(b, 0, BUFFER_SIZE);
  sprintf(b, "DIR: %s", fd->dir);
  response(fd->connfd, RC_CMD_OK, b);

  return SUCC;
}

int cmd_dele(char *arg, struct Socketfd *fd) {
  if (!strlen(arg)) {
    response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'DELE [filename]'.");
    return FAIL;
  }

  // check mode
  if (!check_mode(fd->connfd, fd->mode, RQ_COMN)) {
    return FAIL;
  }

  char tmp[DIR_SIZE];
  memset(tmp, 0, DIR_SIZE);
  sprintf(tmp, "%s/%s", fd->dir, arg);

  if (unlink(tmp) < 0) {
    printf("Error unlink(): %s(%d).\n", strerror(errno), errno);
    response(fd->connfd, RC_ARG_ERR, strerror(errno));
    return FAIL;
  }

  response(fd->connfd, RC_CMD_OK, "Success.");

  return SUCC;
}

int cmd_mkd(char *arg, struct Socketfd *fd) {
  if (!strlen(arg)) {
    response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'MKD [dirname]'.");
    return FAIL;
  }

  // check mode
  if (!check_mode(fd->connfd, fd->mode, RQ_COMN)) {
    return FAIL;
  }

  char tmp[DIR_SIZE];
  memset(tmp, 0, DIR_SIZE);
  sprintf(tmp, "%s/%s", fd->dir, arg);

  if (mkdir(tmp, 0777) < 0) {
    printf("Error mkdir(): %s(%d).\n", strerror(errno), errno);
    response(fd->connfd, RC_ARG_ERR, strerror(errno));
    return FAIL;
  }

  response(fd->connfd, RC_CMD_OK, "Success.");

  return SUCC;
}

int cmd_rmd(char *arg, struct Socketfd *fd) {
  if (!strlen(arg)) {
    response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'RMD [filename]'.");
    return FAIL;
  }

  // check mode
  if (!check_mode(fd->connfd, fd->mode, RQ_COMN)) {
    return FAIL;
  }

  char tmp[DIR_SIZE];
  memset(tmp, 0, DIR_SIZE);
  sprintf(tmp, "%s/%s", fd->dir, arg);

  if (rmdir(tmp) < 0) {
    printf("Error rmdir(): %s(%d).\n", strerror(errno), errno);
    response(fd->connfd, RC_ARG_ERR, strerror(errno));
    return FAIL;
  }

  response(fd->connfd, RC_CMD_OK, "Success.");

  return SUCC;
}

int cmd_rnfr(char *arg, struct Socketfd *fd) {
  if (!strlen(arg)) {
    response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'RNFR [oldname]'.");
    return FAIL;
  }

  // check mode
  if (!check_mode(fd->connfd, fd->mode, RQ_COMN)) {
    return FAIL;
  }

  memset(fd->oldname, 0, NAME_SIZE);
  sprintf(fd->oldname, "%s/%s", fd->dir, arg);

  char b[BUFFER_SIZE];
  memset(b, 0, BUFFER_SIZE);
  sprintf(b, "FILE:'%s' ready to rename.", fd->oldname);
  response(fd->connfd, RC_CMD_OK, b);

  return SUCC;
}

int cmd_rnto(char *arg, struct Socketfd *fd) {

  if (!strlen(arg)) {
    response(fd->connfd, RC_SYNTAX_ERR, "Command syntax error, input as 'RNTO [newname]'.");
    return FAIL;
  }

  // check mode
  if (!check_mode(fd->connfd, fd->mode, RQ_COMN)) {
    return FAIL;
  }

  char tmp[NAME_SIZE];
  memset(tmp, 0, NAME_SIZE);
  sprintf(tmp, "%s/%s", fd->dir, arg);
  if (rename(fd->oldname, tmp) < 0) {
    response(fd->connfd, RC_EXEC_ERR, "Server error, cannot rename file.");
    return FAIL;
  }

  char b[BUFFER_SIZE];
  memset(b, 0, BUFFER_SIZE);
  sprintf(b, "Rename FILE:'%s' to '%s'.", fd->oldname, arg);
  response(fd->connfd, RC_CMD_OK, b);

  memset(fd->oldname, 0, NAME_SIZE);

  return SUCC;
}
