#include "client.h"

// getaddrinfo() -> loop(socket() -> connect())  =>  connfd
int connectAddress(char *servname) {

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

// socket() -> connect() => sockfd
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

// accept() => connfd
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

int createDataSocket(struct Status *status) {
  int datafd;
  if (status->mode == MODE_PORT) {
    datafd = acceptSocket(status->port_transfd);
    if (datafd == FAIL) {
      printf("Error *acceptSocket(): %s(%d)\n", strerror(errno), errno);
    }
  } else if (status->mode == MODE_PASV) {
    datafd = connectSocket(status->pasv_addr, status->pasv_port);
    if (datafd == FAIL) {
      printf("Error connectSocket(): %s(%d)\n", strerror(errno), errno);
    }
  } else {
    datafd = FAIL;
  }

  return datafd;
}

// recv(connfd)
int recvReply(char *buffer, int connfd) {
	if (recv(connfd, buffer, BUFFER_SIZE, 0) < 0) {
		printf("Error recv(rc) from server(%d): %s(%d)\n", connfd, strerror(errno), errno);
		return FAIL;
	}
	return SUCC;
}

// loop (recvReply()) => print()
void printReply(int connfd) {
  char buf[BUFFER_SIZE];
  memset(buf, 0, BUFFER_SIZE);
  recvReply(buf, connfd);
  /*int r = FAIL;
  while (r == FAIL) {
    r = recvReply(buf, connfd);
  }*/
  printf("%s", buf);
}

// fgets()
int readCommand(char *buf, int size) {
  printf("ftp> ");
  fflush(stdout);
  //fflush(stdin);

  memset(buf, 0, size);
  if (fgets(buf, size, stdin) != NULL) {
    char *c = strchr(buf, '\n');
    if (c) *c = '\0';
    return SUCC;
  } else {
    return FAIL;
  }
}


int sendFile(int datafd, int connfd, char *filename) {

  FILE *file = NULL;

  file = fopen(filename, "rb+");
  if(!file) {
    printf("Error fopen(): %s(%d)\n", strerror(errno), errno);
    return FAIL;
  }

  char buf[DATA_SIZE];
  int nread = -1;
  do {
    memset(buf, 0, DATA_SIZE);
    nread = fread(buf, 1, DATA_SIZE, file);

    if (write(datafd, buf, nread) < 0) {
      printf("Error send(): %s(%d)\n", strerror(errno), errno);
      fclose(file);
      return FAIL;
    }
  } while (nread > 0);

  fclose(file);

  return SUCC;
}

int recvFile(int datafd, int connfd, char *filename) {

  FILE *file = NULL;

  file = fopen(filename, "wb");
  if(!file) {
    printf("Error fopen(): %s(%d)\n", strerror(errno), errno);
    return FAIL;
  }

  char buf[DATA_SIZE];
  int nwrite = -1;
  do {
    memset(buf, 0, DATA_SIZE);
    nwrite = read(datafd, buf, DATA_SIZE);
    if (nwrite < 0) {
      printf("Error recv(): %s(%d)\n", strerror(errno), errno);
      fclose(file);
      return FAIL;
    }

    fwrite(buf, nwrite, 1, file);
  } while (nwrite > 0);

  fclose(file);

  return SUCC;
}
