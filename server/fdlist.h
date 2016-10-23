#ifndef FDLIST_H
#define FDLIST_H

#include "common.h"

#define MODE_GUEST  0x00000001
#define MODE_USER   0x00000010
#define MODE_LOGIN  0x00000100
#define MODE_PASV   0x00001000
#define MODE_PORT   0x00010000
#define MODE_RENAME 0x00100000
#define MODE_QUIT   0x01000000

// description of socket
struct Socketfd {
  int connfd;
  int mode;
  
  struct sockaddr_in addr;
  int transfd;

  int iscmd; // is current connfd executing command?
  char dir[DIR_SIZE];
  char oldname[NAME_SIZE];
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
void fdlist_poll(struct FdList *fdlist, fd_set *sockfd);

// is full
int fdlist_isfull(struct FdList *fdlist);

// is contained
int fdlist_isset(struct FdList *fdlist, int sockfd);

// add
int fdlist_add(struct FdList *fdlist, int sockfd);

// del
int fdlist_del(struct FdList *fdlist, int sockfd);

#endif
