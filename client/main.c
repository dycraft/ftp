#include "common.h"
#include "client.h"
#include "command.h"

int handleCommand(struct Command *cmd, struct Status *status);

int main(int argc, char* arg[]) {

  char *host = "127.0.0.1";
  char *port = arg[1];

  // status init
  struct Status status;
  memset(&status, 0, sizeof(status));

  // create socket
  status.connfd = connectSocket(host, atoi(port));
  if (status.connfd == FAIL) {
    printf("Error *connectAddress(): %s(%d)\n", strerror(errno), errno);
    return 1;
  }

  printf("connected to %s.\n", host);

  // recv 220 reply
  printReply(status.connfd);

  // command loop
  while (true) {

    struct Command cmd;
    memset(&cmd, 0, sizeof(cmd));

    // read command
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    if (readCommand(buffer, BUFFER_SIZE) == FAIL) {
      printf("Error *readcmd(): %s(%d)\n", strerror(errno), errno);
      break;
    }

    // send command
    if (send(status.connfd, buffer, strlen(buffer), 0) < 0) {
      printf("Error send(cmd): %s(%d)\n", strerror(errno), errno);
      break;
    }

    // parse command
    if (command_parse(&cmd, buffer) == FAIL) {
      printf("Error *command_parse().\n");
    }

    // handle command
    if (handleCommand(&cmd, &status) == FAIL) {
      printf("Error *handleCommand().\n");
    }

  }

  // close connection safely
  close(status.connfd);
  if (status.datafd > 0) {
    close(status.datafd);
  }

  return 0;
}


int handleCommand(struct Command *cmd, struct Status *status) {

  // first: recieve reply once (handle normal command)
  char buf[BUFFER_SIZE];
  memset(buf, 0, BUFFER_SIZE);
  if (recvReply(buf, status->connfd) == FAIL) {
    return FAIL;
  }
  printf("%s", buf);

  // handle data connection command
  for (int i = 0; i < HANDLE_NUM; i++) {
    if (strcmp(handlelist[i], cmd->name) == 0) {
      if (handler[i](cmd->arg, status) == FAIL) {
        printf("Error %s().\n", cmd->name);
        return FAIL;
      }
      return SUCC;
    }
  }

  return SUCC;
}
