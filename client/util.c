#include "util.h"


int readInput(char *buf, int size) {
  memset(buf, 0, size);

  if (fgets(buf, size, stdin) != NULL) {
    char *c = strchr(buf, '\n');
    if (c) *c = '\0';
    return SUCC;
  } else {
    return FAIL;
  }
}
