#ifndef FDLIST_H
#define FDLIST_H

#include "common.h"

#define MODE_PASV 1213
#define MODE_PORT 1415

struct FdList {
  size_t size;
  int list[FD_SETSIZE];
  int state[FD_SETSIZE];
}

void fdlist_init((struct FdList *) fdlist);
int fdlist_max((struct FdList *) fdlist);
void fdlist_poll((struct FdList *) fdlist, int sockfd);
int fdlist_isfull((struct FdList *) fdlist);
int fdlist_isset((struct FdList *) fdlist, int sockfd);
int fdlist_add((struct FdList *) fdlist, int sockfd);
int fdlist_del((struct FdList *) fdlist, int sockfd);

#endif
