#include "fdlist.h"

// set to 0
void fdlist_init((struct FdList *) fdlist) {
  fdlist->size = 0;
  for (int i = 0; i < FD_SETSIZE; i++) {
    fdlist->list[i] = 0;
    fdlist->state[i] = 0;
  }
}

// traverse the list to find max
int fdlist_max((struct FdList *) fdlist) {
  int max = -1;
  for (int i = 0; i < size; i++) {
    if (fdlist->list[i] > max) {
      max = fdlist->list[i];
    }
  }
  return max;
}

// traverse and FD_SET
void fdlist_poll((struct FdList *) fdlist, (struct fd_set *)sockfd) {
  for (int i = 0; i < size; i++) {
    FD_SET(fdlist->list[i], sockfd);
  }
}

//
int fdlist_isfull((struct FdList *) fdlist) {
  return (fdlist->size < FD_SETSIZE);
}

//
int fdlist_isset((struct FdList *) fdlist, int sockfd) {
  for (int i = 0; i < fdlist->size; i++) {
    if (fdlist->list[i] == sockfd) {
      return true;
    }
  }
  return false;
}

// isfull & isset => add
int fdlist_add((struct FdList *) fdlist, int sockfd) {
  if (fdlist_isfull(fdlist)) {
    return false;
  }
  if (fdlist_isset(fdlist, sockfd)) {
    return false;
  }
  fdlist->list[fdlist->size++] = sockfd;
  return true;
}

// close => remove
int fdlist_del((struct FdList *) fdlist, int sockfd) {
  for (int i = 0; i < fdlist->size; i++) {
    if (fdlist->list[i] == sockfd) {
      close(sockfd);

      fdlist->size -= 1;
      fdlist->list[i] = fdlist[fdlist->size];
      memset(fdlist->list + fdlist->size, 0, sizeof(int));
      return true;
    }
  }
  return false;
}
