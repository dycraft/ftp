#include "server.h"

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
    if (command_parse(ptrcmd, buffer) == FAIL) {
      printf("Error *command_parse().\n");
    }

    return r_recv;
  }
}


// port() -> | pasv() ->
int createDataSocket(struct Socketfd *fd) {
  int datafd;
  if (fd->mode == MODE_PORT) {
    datafd = createPortSocket(&(fd->addr));
    if (datafd == FAIL) {
      printf("Error createPortSocket(): %s(%d)\n", strerror(errno), errno);
    }
  } else if (fd->mode == MODE_PASV) {
    datafd = createPasvSocket(fd->transfd);
    if (datafd == FAIL) {
      printf("Error createPasvSocket(): %s(%d)\n", strerror(errno), errno);
    }
  } else {
    datafd = FAIL;
  }

  return datafd;
}

// socket() -> connect()
int createPortSocket(struct sockaddr_in *addr) {
  int sockfd;
  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockfd < 0) {
    printf("Error socket(): %s(%d)\n", strerror(errno), errno);
    return FAIL;
  }

  if(connect(sockfd, (struct sockaddr *)addr, sizeof(*addr)) < 0) {
    printf("Error connect(): %s(%d)\n", strerror(errno), errno);
    return -1;
  }

  return sockfd;
}

// accept()
int createPasvSocket(int listenfd) {
  return acceptSocket(listenfd);
}

int sendFile(int datafd, int connfd, char *filename) {

  FILE *file = NULL;

  file = fopen(filename, "rb+");
  if(!file) {
    printf("Error fopen(): %s(%d)\n", strerror(errno), errno);
    response(connfd, RC_NO_FILE, "Not found this file or no permission to open.");
    return FAIL;
  }

  char b[BUFFER_SIZE];
  memset(b, 0, BUFFER_SIZE);
  sprintf(b, "Server is ready for sending file:%s ...", filename);
  response(connfd, RC_FILE_OK, b);

  char buf[DATA_SIZE];
  int nread = -1;
  do {
    memset(buf, 0, DATA_SIZE);
    nread = fread(buf, 1, DATA_SIZE, file);

    if (write(datafd, buf, nread) < 0) {
      printf("Error send(): %s(%d)\n", strerror(errno), errno);
      response(connfd, RC_NET_ERR, "Cannot open data connection, connection closed.");
      fclose(file);
      return FAIL;
    }
  } while (nread > 0);

  response(connfd, RC_TRANS_OK, "File transfer successfully.");

  fclose(file);

  return SUCC;
}

int recvFile(int datafd, int connfd, char *filename) {

  FILE *file = NULL;

  file = fopen(filename, "wb");
  if(!file) {
    printf("Error fopen(): %s(%d)\n", strerror(errno), errno);
    response(connfd, RC_NO_FILE, "Not found this file or no permission to open.");
    return FAIL;
  }

  char b[BUFFER_SIZE];
  memset(b, 0, BUFFER_SIZE);
  sprintf(b, "Server is ready for recieving file:%s ...", filename);
  response(connfd, RC_FILE_OK, b);

  char buf[DATA_SIZE];
  int nwrite = -1;
  do {
    memset(buf, 0, DATA_SIZE);
    nwrite = read(datafd, buf, DATA_SIZE);
    if (nwrite < 0) {
      printf("Error recv(): %s(%d)\n", strerror(errno), errno);
      response(connfd, RC_NET_ERR, "Cannot accept data connection, connection closed.");
      fclose(file);
      return FAIL;
    }

    fwrite(buf, nwrite, 1, file);
  } while (nwrite > 0);

  response(connfd, RC_TRANS_OK, "File transfer successfully.");

  fclose(file);

  return SUCC;
}


// send()
int response(int sockfd, int rc, const char *reply) {
  char buf[BUFFER_SIZE];
  memset(buf, 0, BUFFER_SIZE);
  sprintf(buf, "%d %s\r\n", rc, reply);

  if (send(sockfd, buf, BUFFER_SIZE, 0) == -1) {
    printf("Error send(%d) to fd(%d): %s(%d)\n", rc, sockfd, strerror(errno), errno);
    return FAIL;
  } else {
    printf("Send to fd(%d): %s", sockfd, buf);
    return SUCC;
  }
}

// get server's ip
int getip(char *ip) {
  strcpy(ip, "127.0.0.1");
  return SUCC;

  struct ifaddrs *ifAddrStruct = NULL;
  void *tmpAddrPtr = NULL;
  getifaddrs(&ifAddrStruct);

  while (ifAddrStruct != NULL) {
    if (ifAddrStruct->ifa_addr->sa_family == AF_INET) {
      char mask[INET_ADDRSTRLEN];
      void* mask_ptr = &((struct sockaddr_in *)ifAddrStruct->ifa_netmask)->sin_addr;
      inet_ntop(AF_INET, mask_ptr, mask, INET_ADDRSTRLEN);
      if (strcmp(mask, "255.0.0.0") != 0) {
        tmpAddrPtr = &((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
        inet_ntop(AF_INET, tmpAddrPtr, ip, INET_ADDRSTRLEN);
        return SUCC;
      }
    }
    ifAddrStruct = ifAddrStruct->ifa_next;
  }

  return FAIL;
}
