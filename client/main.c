#include "common.h"
#include "client.h"
#include "command.h"

//#define printf(...) fprintf(stderr, __VA_ARGS__)

int handleCommand(struct Command *cmd, struct Status *status);

char *host;
int port;
char *root;

int main(int argc, char* arg[]) {

  host = "127.0.0.1";
  int port = atoi(arg[1]);
  root = DEFAULT_ROOT;

  // status init
  struct Status status;
  memset(&status, 0, sizeof(status));

  // create socket
  status.connfd = connectSocket(host, port);
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
    handleCommand(&cmd, &status);
  }

  // close connection safely
  close(status.connfd);
  if (status.datafd > 0) {
    close(status.datafd);
  }

  return 0;
}


int handleCommand(struct Command *cmd, struct Status *status) {

  // handle data connection command
  for (int i = 0; i < HANDLE_NUM; i++) {
    if (strcmp(handlelist[i], cmd->name) == 0) {
      if (handler[i](cmd->arg, status) == FAIL) {
        return FAIL;
      }
      return SUCC;
    }
  }

  // recieve reply once (cmd out of handlelist)
  char buf[BUFFER_SIZE];
  memset(buf, 0, BUFFER_SIZE);
  if (recvReply(buf, status->connfd) == FAIL) {
    return FAIL;
  }
  printf("%s", buf);

  return SUCC;
}
