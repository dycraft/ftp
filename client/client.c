#include "client.h"
#include "reply.h"

//int port;
int root;

int main(int argc, char* argv[]) {

  char *hostname = "localhost";
  char *servname = "21";

  // reply.h
  reply_init();

  // create socket
  int connfd = connectAddress(hostname, servname);
  if (connfd == FAIL) {
    printf("Error *connectAddress(): %s(%d)\n", strerror(errno), errno);
    return 0;
  }

  printf("connected to %s.\n", hostname);

  // fork a child process to recv response
  int pid = fork();
  if (pid < 0) {
    printf("Error fork(): %s(%d)\n", strerror(errno), errno);
    close(connfd);
    return 0;
  } else if (pid == 0) {
    // child: recieve response (rc) from server

    while (true) {
      int rc = recvReply(connfd);
      if (rc == FAIL) {
        continue;
      }
      printf("%s", reply[rc]);
    }
  } else {
    // parent: handle command

    while (true) {
      char buffer[BUFFER_SIZE];
      // read command
      if (readCmd(buffer, BUFFER_SIZE) == FAIL) {
        printf("Error *readcmd(): %s(%d)\n", strerror(errno), errno);
        continue;
      }
      // send command
      if (send(connfd, buffer, strlen(buffer), 0) == FAIL) {
        printf("Error send(cmd): %s(%d)\n", strerror(errno), errno);
        break; // exit
      }
    }
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

  if (getaddrinfo(host, port, &hints, &res0) == -1) {
    printf("Error getaddrinfo(): %s(%d)\n", strerror(errno), errno);
    return FAIL;
  }

  // try connection in all resources
  int connfd;
  struct addrinfo *res = res0;
  for (res = res0; res; res = res->ai_next) {
    // socket()
    connfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (connfd == -1) {
      printf("Fail in socket() at res loop: %s(%d)\n", strerror(errno), errno);
      continue;
    }

    // connnect()
    if (connect(connfd, res->ai_addr, res->ai_addrlen) < 0) {
      printf("Error connect() in res loop: %s(%d)\n", strerror(errno), errno);
      close(connfd);
      connfd = FAIL;
      continue;
    } else {
      // if success
      break;
    }
  }
  freeaddrinfo(res0);

  return connfd;
}

int recvReply(int connfd) {
  int rc = 0;
	if (recv(sock_control, &rc, sizeof(rc), 0) < 0) {
		printf("Error recv(rc) from server: %s(%d)\n", strerror(errno), errno);
		return FAIL;
	}
	return ntohl(rc);
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
