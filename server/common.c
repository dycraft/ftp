#include "common.h"

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
