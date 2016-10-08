#include "server.h"
#include "reply.h"
#include "fdlist.h"
#include "command.h"

int main(int argc, char *argv[]) {

  int port = atoi(argv[1]);

  // createa passive socket
  int listenfd;
  if ((listenfd = createSocket(port)) == -1) {
    printf("Error createSocket(): %s(%d)\n", strerror(errno), errno);
    exit(1);
  }

  prinf("Server start at port: %d, waiting for client...", port);

  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;

  struct fd_set readfd;
  struct FdList fdlist;
  fdlist_init(&fdlist);

  while (true) {

    FD_ZERO(&readfd);
    FD_SET(listenfd, &readfd); // add the first: listenfd
    fdlist_poll(&fdlist, &readfd); // add the rest

    if (Select(fdlist_max(&fdlist) + 1, &readfd, NULL, NULL, &timeout) <= 0) {
      continue;
    }

    // accept connection from client
    int connfd;
    if (FD_ISSET(listenfd, &readfd)) {
      connfd = acceptSocket(listenfd);
      if (connfd < 0) {
        printf("Error acceptSocket(): %s(%d)\n", strerror(errno), errno);
      }

      fdlist_add(&fdlist, connfd);
      response(connfd, RC_LOGIN);

      printf("Server accept client's connection.");
    }

    //parse command
    for (int i = 0; i < fdlist->size; i++) {
      if (FD_ISSET(fdlist->list[i], &readfd)) {
        struct Command cmd;
        memset(&cmd, 0, sizeof(cmd));
        int rc = recvCommand(fdlist->list[i], cmd);
        printf("cmd: %s", reply[rc]);

        // exec cmd in pthread
        pthread_t tid;
        void *arg[] = { &cmd, &(fdlist->list[i]) };
        if (pthread_create(&tid, NULL, p_executeCommand, arg) == 0) {
          printf("Error pthread_create(): %s(%d), command failed.\n", strerror(errno), errno);
        }
      }
    }
  }

  close(listenfd);

  printf("Server closed.");

  return 0;
}

/*********************************
*
*     wrapper for socket
*
*********************************/

// socket() -> setsockopt() -> bind() -> listen()  =>  listenfd
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

// accept()  =>  connfd
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

//
// socket() -> connect()  =>
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

  if(connect(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
    printf("Error connect(): %s(%d)\n", strerror(errno), errno);
    return -1;
  }

  return sockfd;
}

// send() with reply code
int response(int sockfd, int rc) {
  int rc_n = htonl(rc);
  printf("Send reply code: %d", rc);

  if (send(sockfd, &rc_n, sizeof(rc_n), 0) == -1) {
    printf("Error send(): %s(%d)\n", strerror(errno), errno);
    return FAIL;
  }
  return SUCC;
}

// recv() -> send(rc)
int recvCommand(int connfd, (struct Command *) ptrcmd) {

  char buffer[BUFFER_SIZE];
  memset(buffer, 0, BUFFER_SIZE);

  int r_recv = recv(fdlist->list[i], buffer, BUFFER_SIZE, MSG_DONTWAIT);
  if (r_recv == -1) {
    fdlist_del(&fdlist, fdlist->list[i]);
    printf("Error recv(): %s(%d), timeout.\n", strerror(errno), errno);
    return FAIL;
  } else if (r_recv == 0){
    fdlist_del(&fdlist, fdlist->list[i]);
    printf("Error recv(): %s(%d), client disconnect.\n", strerror(errno), errno);
    return FAIL;
  } else {
    printf("Recieve command: %s", buffer);

    // parse command
    memset(ptrcmd, 0, sizeof(*ptrcmd));
    strncpy(ptrcmd->name, buf, 4);
    strcpy(ptrcmd->arg, buf+5);

    // send command status
    response(connfd, RC_CMD_OK);

    return rc;
  }
}


void *p_executeCommand(void *arg) {
  struct Command cmd = *(struct Command *)arg[0];
  struct int connfd = *(int *)arg[1];



  return NULL;
}
