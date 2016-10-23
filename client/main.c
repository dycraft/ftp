#include "common.h"
#include "client.h"
#include "command.h"
#include "util.h"

int handleCommand(struct Command *cmd, struct Status *status);

// check command line arguments
int handleCliArg(int argc, char *arg[]);

char *host;
int port;
char *root;

int main(int argc, char* arg[]) {

  host = "127.0.0.1";
  // check command line arguments
  if (handleCliArg(argc, arg) == FAIL) {
   printf("Parameters Error. Input as:\n./client [-port PORT] [-root DIR]\n");
   return 1;
  }

  // status init
  struct Status status;
  memset(&status, 0, sizeof(status));
  status.mode = MODE_NORM;

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
    command_parse(&cmd, buffer);

    // handle command
    handleCommand(&cmd, &status);
  }

  // close connection safely
  close(status.connfd);

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


int handleCliArg(int argc, char *arg[]) {
  if ((argc != 1) && (argc != 3) && (argc != 5)) {
    return FAIL;
  }

  int n_root = 0;
  int n_port = 0;

  for (int i = 1; i <= argc-2; i += 2) {
    if (strcmp(arg[i], "-port") == 0) {
      n_port = i + 1;
    }
    if (strcmp(arg[i], "-root") == 0) {
      n_root = i + 1;
    }
  }

  if (n_port == 0) {
    port = DEFAULT_PORT;
  } else {
    port = atoi(arg[n_port]);
  }

  if (n_root == 0) {
    root = DEFAULT_ROOT;
  } else {
    root = arg[n_root];
  }

  if (root[1] == '.') {
    printf("Sorry, we could not provide permission for other folders.\n");
    return FAIL;
  }

  return SUCC;
}
