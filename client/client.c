#include "client.h"

int main(int argc, char* argv[]) {

  char *host = "127.0.0.1";
  char *port = argv[1];

  // create socket
  int connfd = connectSocket(host, atoi(port));
  if (connfd == FAIL) {
    printf("Error *connectAddress(): %s(%d)\n", strerror(errno), errno);
    return 1;
  }

  printf("connected to %s.\n", host);

  showReply(connfd);

  while (true) {

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    // read command
    if (readCommand(buffer, BUFFER_SIZE) == FAIL) {
      printf("Error *readcmd(): %s(%d)\n", strerror(errno), errno);
      break;
    }
    // send command
    if (send(connfd, buffer, strlen(buffer), 0) == FAIL) {
      printf("Error send(cmd): %s(%d)\n", strerror(errno), errno);
      break; // exit
    }
    // recieve reply
    showReply(connfd);
  }

  close(connfd);

  return 0;
}

// getaddrinfo() -> loop(socket() -> connect())  =>  connfd
int connectAddress(char *hostname, char *servname) {

  // getaddrinfo()
  struct addrinfo hints, *res0;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(NULL, servname, &hints, &res0) == -1) {
    printf("Error getaddrinfo(): %s(%d)\n", strerror(errno), errno);
    return FAIL;
  }

  // try connection in all resources
  int connfd;
  struct addrinfo *res = res0;
  for (res = res0; res; res = res->ai_next) {
    // socket()
    connfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (connfd == -1) {
      printf("Fail in socket() at res loop: %s(%d)\n", strerror(errno), errno);
      continue;
    }

    // connnect()
    if (connect(connfd, res->ai_addr, res->ai_addrlen) < 0) {
      printf("Fail in connect() in res loop: %s(%d)\n", strerror(errno), errno);
      close(connfd);
      connfd = FAIL;
      continue;
    } else {
      // if success
      printf("Connect server successfully.\n");
      break;
    }
  }
  freeaddrinfo(res0);

  return connfd;
}

int connectSocket(char *host, int port) {

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		printf("Error socket(): %s(%d)\n", strerror(errno), errno);
		return 1;
	}

  struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(host);

  int connfd = connect(sockfd, (struct sockaddr *) &addr, sizeof(addr));
	if (connfd < 0) {
		printf("Error connect(): %s(%d)\n", strerror(errno), errno);
		return FAIL;
	}

  return sockfd;
}

int recvReply(char *buffer, int connfd) {
	if (recv(connfd, buffer, BUFFER_SIZE, 0) < 0) {
		printf("Error recv(rc) from server(%d): %s(%d)\n", connfd, strerror(errno), errno);
		return FAIL;
	}
	return SUCC;
}

void showReply(int connfd) {
  char buf[BUFFER_SIZE];
  memset(buf, 0, BUFFER_SIZE);
  int r = FAIL;
  while (r == FAIL) {
    r = recvReply(buf, connfd);
  }
  printf("%s", buf);
}


int readCommand(char *buf, int size) {
  printf("ftp> ");
  fflush(stdout);

  memset(buf, 0, size);
  if (fgets(buf, size, stdin) != NULL) {
    char *c = strchr(buf, '\n');
    if (c) *c = '\0';
    return SUCC;
  } else {
    return FAIL;
  }
}
