#include "server.h"
#include "reply.h"
#include "fdlist.h"

// fdlist del删除的时候需要同时调用FD_CLR？
// cmd 发送过来后，有且仅有一个response?

int port;
char *root;

int main(int argc, char *argv[]) {

  // check command line arguments
  if (handleCliArg(argc, argv) == FAIL) {
    printf("Parameters Error. Input as\n\t./ftpserver [-port PORT] [-root DIR]\n");
    return 0;
  }

  // reply.h
  reply_init();

  // createa passive socket
  int listenfd;
  if ((listenfd = createSocket(port)) == -1) {
    printf("Error createSocket(): %s(%d)\n", strerror(errno), errno);
    exit(1);
  }

  printf("Server start at port: %d, waiting for client...", port);

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

    if (select(fdlist_max(&fdlist) + 1, &readfd, NULL, NULL, &timeout) <= 0) {
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
      response(connfd, RC_NEW_USER);

      printf("Server accept client's connection.");
    }

    //parse command
    for (int i = 0; i < fdlist.size; i++) {
      if (FD_ISSET(fdlist.list[i], &readfd)) {
        struct Command cmd;
        command_init(&cmd);
        int r_del = recvCommand(fdlist.list[i], &cmd);
        if (r_del != SUCC) {
          fdlist_del(&fdlist, r_del);
        }

        // exec cmd in pthread
        pthread_t tid;
        void *arg[] = { &(fdlist.list[i]), &cmd };
        if (pthread_create(&tid, NULL, p_executeCommand, &arg) == 0) {
          printf("Error pthread_create(): %s(%d), command failed.\n", strerror(errno), errno);
        }
        FD_CLR(fdlist.list[i], &readfd);
        fdlist_del(&fdlist, fdlist.list[i]);
      }
    }
  }

  close(listenfd);

  printf("Server closed.");

  return 0;
}

int handleCliArg(int argc, char *argv[]) {
  if ((argc != 1) && (argc != 3) && (argc != 5)) {
    return FAIL;
  }

  int n_root = 0;
  int n_port = 0;

  for (int i = 1; i <= argc-2; i += 2) {
    if (strcmp(argv[i], "-port") == 0) {
      n_port = i + 1;
    }
    if (strcmp(argv[i], "-root") == 0) {
      n_root = i + 1;
    }
  }

  if (n_port == 0) {
    port = DEFAULT_PORT;
  } else {
    port = atoi(argv[n_port]);
  }

  if (n_root == 0) {
    root = DEFAULT_ROOT;
  } else {
    root = argv[n_root];
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

  int r_recv = recv(connfd, buffer, BUFFER_SIZE, MSG_DONTWAIT);
  if (r_recv == -1) {
    printf("Error recv(): %s(%d), timeout.\n", strerror(errno), errno);
    return connfd;
  } else if (r_recv == 0){
    printf("Error recv(): %s(%d), client disconnect.\n", strerror(errno), errno);
    return connfd;
  } else {
    printf("Recieve command: %s", buffer);

    // parse command
    command_parse(ptrcmd, buffer);

    return SUCC;
  }
}


void *p_executeCommand(void *arg) {
  // a dirty way to pass arg and decode arg
  int connfd = *((int *)(arg));
  struct Command cmd = *((struct Command *)(arg + sizeof(int)));

  for (int i = 0; i < CMD_NUM; i++) {
    if (cmdlist[i] == cmd.name) {
      if (execlist[i](, cmd.argv, connfd) == FAIL) {
        printf("Error %s(): %s(%d).\n", cmdlist[i], strerror(errno), errno);
      }
      return NULL;
    }
  }

  // command not implemented
  response(connfd, RC_NO_IMP);

  return NULL;
}
