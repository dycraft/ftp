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

      //do something

			close(connfd);
			exit(0);
    }

    close(connfd);
  }

  close(listenfd);

  return 0;
}
