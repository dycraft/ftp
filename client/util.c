#include "util.h"

int randPort(int seed) {
  srand(seed);
  return rand() % (65536 - 20000) + 20000;
}

int getip(char *ip) {
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


int parseRC(char *reply) {
  int rc = 0;
  int num = sscanf(reply, "%d", &rc);
  if (num != 1) {
    return FAIL;
  } else {
    return rc;
  }
}


int decodeAddress(char *addr, int *port, char *buf) {
  int h1, h2, h3, h4, p1, p2;
  int num = sscanf(buf, "%d,%d,%d,%d,%d,%d", &h1, &h2, &h3, &h4, &p1, &p2);
  if (num != 6) {
    return FAIL;
  }

  // port
  if ((p1 >= 0) && (p2 >= 0) && (p1 < 256) && (p2 < 256)) {
    *port = p1 * 256 + p2;
  } else {
    return FAIL;
  }

  // addr
  sprintf(addr, "%d.%d.%d.%d", h1, h2, h3, h4);

  return SUCC;
}

int encodeAddress(char *buf, char *addr, int port) {
  int h1, h2, h3, h4, p1, p2;
  int num = sscanf(addr, "%d.%d.%d.%d", &h1, &h2, &h3, &h4);
  if (num != 4) {
    return FAIL;
  }

  p1 = port / 256;
  p2 = port % 256;

  sprintf(buf, "%d,%d,%d,%d,%d,%d", h1, h2, h3, h4, p1, p2);

  return SUCC;
}
