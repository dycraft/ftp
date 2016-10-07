#include "server.h"

int main(int argc, char *argv[]) {

  int port = atoi(argv[1]);

  int listenfd;
  if ((listenfd = createSocket(port)) == -1) {
    printf("Error createSocket(): %s(%d)\n", strerror(errno), errno);
    exit(1);
  }

  int connfd, pid;
  while (true) {
    connfd = acceptSocket(listenfd);
    if (connfd == -1) {
      break;
    }

    pid = fork();
    if (pid < 0) {
      printf("Error fork(): %s(%d)\n", strerror(errno), errno);
    } else if (pid == 0) {
      // child process
      close(listenfd);

      handleConnection(connfd);

      close(connfd);
      exit(0);
    }

    close(connfd);
  }

  close(listenfd);

  return 0;
}

/*
* wrapper for socket
*/

int createSocket(int port) {

  int sockfd;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    printf("Error socket(): %s(%d)\n", strerror(errno), errno);
    return -1;
  }

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  int optval = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1) {
    close(sockfd);
    printf("Error setsockopt(): %s(%d)\n", strerror(errno), errno);
    return -1;
  }

  if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    close(sockfd);
    printf("Error bind(): %s(%d)\n", strerror(errno), errno);
    return -1;
  }

  if (listen(sockfd, 10) == -1) {
    close(sockfd);
    printf("Error listen(): %s(%d)\n", strerror(errno), errno);
    return -1;
  }

  return sockfd;
}

int acceptSocket(int listenfd) {

  int sockfd;
  struct sockaddr_in addr;
  int len = sizeof(addr);

  sockfd = accept(listenfd, (struct sockaddr *) &addr, (socklen_t *) &len);
  if (sockfd == -1) {
    printf("Error accept(): %s(%d)\n", strerror(errno), errno);
    return -1;
  }

  return sockfd;
}

int connectSocket(int port, char *host) {

  int sockfd;
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    printf("Error socket(): %s(%d)\n", strerror(errno), errno);
    return -1;
  }

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(host);

  if(connect(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0 ) {
    printf("Error connect(): %s(%d)\n", strerror(errno), errno);
    return -1;
  }

  return sockfd;
}

// Child process
void handleConnection(int connfd) {
  // welcome
  response(connfd, 220);

  // user


  while (true) {
    struct Command cmd;
    int rc = recvCommand(connfd, cmd);
    if ((rc == 221) || (rc < 0)) {
      break;
    } else if (rc == 200) {

    }
  }
}

int connectDataSocket(int connfd) {
  //todo
}

// reply code
int response(int sockfd, int rc) {
  int rc_n = htonl(rc);
  if (send(sockfd, &rc_n, sizeof(rc_n), 0) == -1) {
    printf("Error send(): %s(%d)\n", strerror(errno), errno);
    return FAIL;
  }
  return SUCC;
}

// recieve command
int recvCommand(int connfd, (struct Command *) ptrcmd) {

  char buffer[BUFFER_SIZE];

  memset(buffer, 0, BUFFER_SIZE);

  if (recv(connfd, buffer, BUFFER_SIZE, 0) == -1) {
    printf("Error recv(): %s(%d)\n", strerror(errno), errno);
    return FAIL;
  }

  memset(ptrcmd, 0, sizeof(*ptrcmd));
  strncpy(ptrcmd->name, buf, 4);
  strcpy(ptrcmd->arg, buf+5);

  int rc = 200;
  if (strcmp(cmd, "QUIT") == 0) {
    rc = 221;
  } else {
    rc = 500;
  }
  response(connfd, rc);

  return rc;
}
