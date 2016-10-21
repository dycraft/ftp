#include "fdlist.h"

void socketfd_init(struct Socketfd *fd) {
  memset(fd, 0, sizeof(*fd));
}

void socketfd_copy(struct Socketfd *dstfd, struct Socketfd *srcfd) {
  socketfd_init(dstfd);
  dstfd->connfd = srcfd->connfd;
  dstfd->mode = srcfd->mode;
  dstfd->transfd = srcfd->transfd;
  dstfd->iscmd = srcfd->iscmd;
  dstfd->addr.sin_family = srcfd->addr.sin_family;
  dstfd->addr.sin_port = srcfd->addr.sin_port;
  dstfd->addr.sin_addr.s_addr = srcfd->addr.sin_addr.s_addr;
}


// set to 0
void fdlist_init(struct FdList *fdlist) {
  memset(fdlist, 0, sizeof(*fdlist));
}

//
int fdlist_max(struct FdList *fdlist) {
  int max = -1;
  for (int i = 0; i < fdlist->size; i++) {
    if (fdlist->list[i].connfd > max) {
      max = fdlist->list[i].connfd;
    }
  }
  return max;
}

// FD_SET
void fdlist_poll(struct FdList *fdlist, fd_set *sockfd) {
  for (int i = 0; i < fdlist->size; i++) {
    FD_SET(fdlist->list[i].connfd, sockfd);
  }
}

//
int fdlist_isfull(struct FdList *fdlist) {
  return (fdlist->size >= FD_SETSIZE);
}

//
int fdlist_isset(struct FdList *fdlist, int sockfd) {
  for (int i = 0; i < fdlist->size; i++) {
    if (fdlist->list[i].connfd == sockfd) {
      return true;
    }
  }
  return false;
}

// isfull & isset => add
int fdlist_add(struct FdList *fdlist, int sockfd) {
  if (fdlist_isfull(fdlist)) {
    return FAIL;
  }
  if (fdlist_isset(fdlist, sockfd)) {
    return FAIL;
  }
  socketfd_init(&fdlist->list[fdlist->size]);
  fdlist->list[fdlist->size].connfd = sockfd;
  fdlist->list[fdlist->size].mode = MODE_GUEST;
  fdlist->size++;
  return SUCC;
}

// close => remove
int fdlist_del(struct FdList *fdlist, int sockfd) {
  for (int i = 0; i < fdlist->size; i++) {
    if (fdlist->list[i].connfd == sockfd) {
      fdlist->size -= 1;
      if (i != fdlist->size) {
        socketfd_copy(&fdlist->list[i], &fdlist->list[fdlist->size]);
        socketfd_init(&fdlist->list[fdlist->size]);
      } else {
        socketfd_init(&fdlist->list[fdlist->size]);
      }
      return SUCC;
    }
  }
  return FAIL;
}
