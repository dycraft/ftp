#include "server.h"
#include "fdlist.h"

//#define printf(...) fprintf(stderr, __VA_ARGS__)

int port;
char *root;

// check command line arguments
int handleCliArg(int argc, char *arg[]);

// mutithread to execute command
void *p_executeCommand(void *arg);

struct thread_arg {
  struct Command *cmd;
  struct Socketfd *fd;
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

  fd_set readfd;
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
      if (FD_ISSET(fdlist.list[i].connfd, &readfd)) {
        // if fd is executing command in another thread
        if (fdlist.list[i].iscmd == true) {
          continue;
        }

        struct Command cmd;
        memset(&cmd, 0, sizeof(struct Command));
        int r = recvCommand(fdlist.list[i].connfd, &cmd);
        if (r == 0) {
          // disconnect
          FD_CLR(fdlist.list[i].connfd, &readfd);
          close(fdlist.list[i].connfd);
          fdlist_del(&fdlist, fdlist.list[i].connfd);
        } else if (r == -1){
          // timeout
          continue;
        } else {
          // exec cmd in pthread
          pthread_t tid;
          struct thread_arg arg;
          memset(&arg, 0, sizeof(arg));
          arg.fd = &fdlist.list[i];
          arg.cmd = &cmd;

          if (pthread_create(&tid, NULL, p_executeCommand, (void *)&arg) != 0) {
            printf("Error pthread_create(): %s(%d), command failed.\n", strerror(errno), errno);
            return 1;
          }
        }
      } else {

      }
    }

    // handle quit fd
    for (int i = 0; i < fdlist.size; i++) {
      if (FD_ISSET(fdlist.list[i].connfd, &readfd)) {
        if (fdlist.list[i].mode == MODE_QUIT) {
          FD_CLR(fdlist.list[i].connfd, &readfd);
          close(fdlist.list[i].connfd);
          fdlist_del(&fdlist, fdlist.list[i].connfd);
          break;
        }
      }
    }
  }

  close(listenfd);

  printf("Server closed.\n");

  return 0;
}

void *p_executeCommand(void *arg) {
  struct Command *cmd = ((struct thread_arg *)arg)->cmd;
  struct Socketfd *fd = ((struct thread_arg *)arg)->fd;

  fd->iscmd = true;

  for (int i = 0; i < CMD_NUM; i++) {
    if (strcmp(cmdlist[i], cmd->name) == 0) {
      if (fd->mode == MODE_GUEST && i != CMD_USER && i != CMD_QUIT) {
        // not login
        response(fd->connfd, RC_NOT_LOG, "Not login.");
        fd->iscmd = false;
        return NULL;
      } else {
        if (execlist[i](cmd->arg, fd) == FAIL) {
          printf("Error %s().\n", cmdlist[i]);
        }
        fd->iscmd = false;
        return NULL;
      }
    }
  }

  // invalid command
  response(fd->connfd, RC_NO_IMP, "?Invalid Command.");

  fd->iscmd = false;
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

  if (root[0] == '.') {
    printf("Sorry, we could not provide permission for other folders.\n");
    return FAIL;
  }

  return SUCC;
}
