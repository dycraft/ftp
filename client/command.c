#include "command.h"
#include "client.h"
#include "util.h"

/* extern global variables init */

char *handlelist[] = {
  "QUIT",
  "PORT",
  "PASV",
  "RETR",
  "STOR",
  "LIST"
};

int (*handle[])() = {
  &handle_quit,
  &handle_port,
  &handle_pasv,
  &handle_retr,
  &handle_stor,
  &handle_list
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


int handle_quit(char *arg, char *reply, struct Status *status) {
  close(status->connfd);
  if (status->datafd > 0) {
    close(status->datafd);
  }
  exit(0);
}

int handle_port(char *arg, char *reply, struct Status *status) {

  // decode address in reply
  char addr[20];
  int port = 0;
  if (decodeAddress(addr, &port, arg) == FAIL) {
    printf("Error *decodeAddress().");
    return FAIL;
  }
  // creata listenfd(transfd)
  status->port_transfd = createSocket(port);

  // update info
  status->pasv_port = 0;
  status->pasv_addr = NULL;
  status->mode = MODE_PASV;

  return SUCC;
}

int handle_pasv(char *arg, char *reply, struct Status *status) {

  // decode address in reply
  char buf[ARG_LEN], temp[BUFFER_SIZE];
  sscanf(reply, "%[^(](%[^)])", temp, buf); // convert the format to "h1,h2,h3,h4,p1,p2"
  char addr[20];
  int port = 0;
  if (decodeAddress(addr, &port, buf) == FAIL) {
    printf("Error *decodeAddress().");
    return FAIL;
  }

  // update info
  status->pasv_port = port;
  status->pasv_addr = addr;
  close(status->port_transfd);
  status->port_transfd = 0;
  status->mode = MODE_PORT;

  return SUCC;
}

int handle_retr(char *arg, char *reply, struct Status *status) {

  // get rc
  int rc = parseRC(reply);
  if (rc == FAIL) {
    printf("Error *parseRC().\n");
    return FAIL;
  }

  // first response
  if (rc != RC_FILE_OK) {
    return FAIL;
  }

  int datafd = createDataSocket(status);
  if (datafd == FAIL) {
    return FAIL;
  }

  // data translation
  char buf[BUFFER_SIZE];
  memset(buf, 0, BUFFER_SIZE);
  strcpy(buf, root);
  strcat(buf, arg);
  if (recvFile(datafd, status->connfd, buf) == FAIL) {
    printf("Error *recvFile(%d, %s).", datafd, buf);
  }

  close(datafd);

  // second response
  if (rc != RC_TRANS_OK) {
    return FAIL;
  }

  return SUCC;
}

int handle_stor(char *arg, char *reply, struct Status *status) {

  // get rc
  int rc = parseRC(reply);
  if (rc == FAIL) {
    printf("Error *parseRC().\n");
    return FAIL;
  }

  // first response
  if (rc != RC_FILE_OK) {
    return FAIL;
  }

  int datafd = createDataSocket(status);
  if (datafd == FAIL) {
    return FAIL;
  }

  // data translation
  char buf[BUFFER_SIZE];
  memset(buf, 0, BUFFER_SIZE);
  strcpy(buf, root);
  strcat(buf, arg);
  if (sendFile(datafd, status->connfd, buf) == FAIL) {
    printf("Error *sendFile(%d, %s).", datafd, buf);
  }

  close(datafd);

  // second response
  if (rc != RC_TRANS_OK) {
    return FAIL;
  }

  return SUCC;
}

int handle_list(char *arg, char *reply, struct Status *status) {

  return SUCC;
}
