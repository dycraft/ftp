#include "server.h"
#include "fdlist.h"

int port;
char *root;

struct threadArg {
  struct Command *cmd;
  int connfd;
};

int main(int argc, char *arg[]) {

  // check command line arguments
  if (handleCliArg(argc, arg) == FAIL) {
    printf("Parameters Error. Input as:\n./ftpserver [-port PORT] [-root DIR]\n");
    return 1;
  }

  // createa passive socket
  int listenfd = createSocket(port);
  if (listenfd == FAIL) {
    printf("Error createSocket(): %s(%d)\n", strerror(errno), errno);
    return 1;
  }

  printf("Server start at port: %d, waiting for client...\n", port);

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

    int maxfd = max(fdlist_max(&fdlist), listenfd);
    if (select(maxfd + 1, &readfd, NULL, NULL, &timeout) <= 0) {
      continue;
    }

    // accept connection from a new client
    int connfd;
    if (FD_ISSET(listenfd, &readfd)) {
      connfd = acceptSocket(listenfd);
      if (connfd < 0) {
        printf("Error acceptSocket(): %s(%d)\n", strerror(errno), errno);
        close(listenfd);
        return 1;
      }

      printf("Server accept client(%d)'s connection.\n", connfd);
      if (fdlist_add(&fdlist, connfd) == FAIL) {
        printf("Error fdlist_add().\n");
      }
      response(connfd, RC_NEW_USER, "Server ready for new user.");
    }

    //parse command
    for (int i = 0; i < fdlist.size; i++) {
      if (FD_ISSET(fdlist.list[i], &readfd)) {
        struct Command cmd;
        memset(&cmd, 0, sizeof(struct Command));
        int r = recvCommand(fdlist.list[i], &cmd);
        if (r == 0) {
          // disconnect
          FD_CLR(fdlist.list[i], &readfd);
          fdlist_del(&fdlist, fdlist.list[i]);
          close(fdlist.list[i]);
        } else if (r == -1){
          // timeout
          continue;
        } else {
          // exec cmd in pthread
          pthread_t tid;
          struct threadArg arg;
          memset(&arg, 0, sizeof(arg));
          arg.connfd = fdlist.list[i];
          arg.cmd = &cmd;

          if (pthread_create(&tid, NULL, p_executeCommand, &arg) != 0) {
            printf("Error pthread_create(): %s(%d), command failed.\n", strerror(errno), errno);
            return 1;
          }
        }
      }
    }
  }

  close(listenfd);

  printf("Server closed.\n");

  return 0;
}

void *p_executeCommand(void *arg) {
  struct Command *cmd = ((struct threadArg *)arg)->cmd;
  int connfd = ((struct threadArg *)arg)->connfd;
  printf("BB:%s\n", cmd->name);
  for (int i = 0; i < CMD_NUM; i++) {
    if (strcmp(cmdlist[i], cmd->name) == 0) {
      if (execlist[i](cmd->arg, connfd) == FAIL) {
        printf("Error %s().\n", cmdlist[i]);
      }
      return NULL;
    }
  }

  // invalid command
  response(connfd, RC_NO_IMP, "?Invalid Command.");

  return NULL;
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

  return SUCC;
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

// recv() -> send(rc)
int recvCommand(int connfd, struct Command *ptrcmd) {

  char buffer[BUFFER_SIZE];
  memset(buffer, 0, BUFFER_SIZE);

  int r_recv = recv(connfd, buffer, BUFFER_SIZE, 0);
  if (r_recv == -1) {
    printf("Error recv(): %s(%d), timeout.\n", strerror(errno), errno);
    return FAIL;
  } else if (r_recv == 0){
    printf("Error recv(): %s(%d), client disconnect.\n", strerror(errno), errno);
    return 0;
  } else {
    printf("Recieve command: %s\n", buffer);
    // parse command
    command_parse(ptrcmd, buffer);

    return r_recv;
  }
}
