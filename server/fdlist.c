#include "fdlist.h"

// set to 0
void fdlist_init(struct FdList *fdlist) {
  memset(fdlist, 0, sizeof(*fdlist));
}

//
int fdlist_max(struct FdList *fdlist) {
  int max = -1;
  for (int i = 0; i < fdlist->size; i++) {
    if (fdlist->list[i] > max) {
      max = fdlist->list[i];
    }
  }
  return max;
}

// FD_SET
void fdlist_poll(struct FdList *fdlist, struct fd_set *sockfd) {
  for (int i = 0; i < fdlist->size; i++) {
    FD_SET(fdlist->list[i], sockfd);
  }
}

//
int fdlist_isfull(struct FdList *fdlist) {
  return (fdlist->size >= FD_SETSIZE);
}

//
int fdlist_isset(struct FdList *fdlist, int sockfd) {
  for (int i = 0; i < fdlist->size; i++) {
    if (fdlist->list[i] == sockfd) {
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
  fdlist->list[fdlist->size] = sockfd;
  fdlist->mode[fdlist->size] = MODE_GUEST;
  fdlist->size++;
  return SUCC;
}

// close => remove
int fdlist_del(struct FdList *fdlist, int sockfd) {
  for (int i = 0; i < fdlist->size; i++) {
    if (fdlist->list[i] == sockfd) {
      fdlist->size -= 1;
      fdlist->list[i] = fdlist->list[fdlist->size];
      fdlist->mode[i] = fdlist->mode[fdlist->size];
      fdlist->list[i] = 0;
      fdlist->mode[i] = 0;
      return true;
    }
  }
  return false;
}
