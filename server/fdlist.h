#ifndef FDLIST_H
#define FDLIST_H

#include "common.h"

#define MODE_GUEST  1617
#define MODE_PASS 1819
#define MODE_LOGIN  1111
#define MODE_PASV 1213
#define MODE_PORT 1415
#define MODE_QUIT 2222

// description of socket
struct Socketfd {
  int connfd;
  int mode;
  struct sockaddr_in addr;
  int transfd;
};

// init
void socketfd_init(struct Socketfd *fd);

// copy
void socketfd_copy(struct Socketfd *dstfd, struct Socketfd *srcfd);

// description of fdset
struct FdList {
  size_t size;
  struct Socketfd list[FD_SETSIZE];
};

// init
void fdlist_init(struct FdList *fdlist);

// get max
int fdlist_max(struct FdList *fdlist);

// poll out the fd_set into fdlist
void fdlist_poll(struct FdList *fdlist, struct fd_set *sockfd);

// is full
int fdlist_isfull(struct FdList *fdlist);

// is contained
int fdlist_isset(struct FdList *fdlist, int sockfd);

// add
int fdlist_add(struct FdList *fdlist, int sockfd);

// del
int fdlist_del(struct FdList *fdlist, int sockfd);

#endif
