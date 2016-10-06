#include "util.h"

#include <stdio.h>

int readinput(char *buf, int size) {
  memset(buffer, 0, size);

  if (fgets(buffer, size, stdin) != NULL) {
    char *c = strchr(buffer, '\n');
    if (c) *c = '\0';
    return SUCC;
  } else {
    return FAIL;
  }
}
