#ifndef FDLIST_H
#define FDLIST_H

#include "common.h"

#define MODE_PASV 1213
#define MODE_PORT 1415

// description of fdset
struct FdList {
  size_t size;
  int list[FD_SETSIZE];
  int state[FD_SETSIZE];
}

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
