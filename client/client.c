#include "client.h"
#include "util.h"

#include <netdb.h>

int connfd;

int main(int argc, char* argv[]) {

  char *host = "localhost";
  char *port = "21";

  struct addrinfo hints, *res;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(host, port, &hints, &res) == -1) {
    printf("Error getaddrinfo(): %s(%d)\n", strerror(errno), errno);
    exit(1);
  }

  struct addrinfo *rp = res;
  while (rp != NULL) {
    connfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (connfd == -1) {
      printf("Error socket(): %s(%d)\n", strerror(errno), errno);
      continue;
    }

    if (connect(connfd, res->ai_addr, res->ai_addrlen) == 0) {
      break;
    } else {
      printf("Error connect(): %s(%d)\n", strerror(errno), errno);
      exit(1);
    }

    close(connfd);

    rp = rp->ai_next;
  }
  freeaddrinfo(rp);

  printf("connected to %s.\n", host);

  while (true) {
    char buffer[BUFFER_SIZE];
    struct command cmd;
    if (readCmd(buffer, sizeof(buffer), &cmd) == -1) {
      printf("Error *readcmd(): %s(%d)\n", strerror(errno), errno);
      continue;
    }

    if (send(connfd, buffer, (size_t)strlen(buffer), 0) == -1) {
      close(connfd);
      exit(1);
    }
  }

  close(connfd);

  return 0;
}


int readCommand(char *buf, int size, struct command* ptrcmd) {
  printf("ftp> ");
  fflush(stdout);

  int ret = readInput(buf, size);
  if (ret == 0) {
    memset(ptrcmd, 0, sizeof(*ptrcmd));
    strncpy(ptrcmd->name, buf, 4);
    strcpy(ptrcmd->arg, buf+5);
    return SUCC;
  } else {
    return FAIL;
  }
}
