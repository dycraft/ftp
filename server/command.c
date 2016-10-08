#include "command.h"

int cmd_user(int argc, char *argv[], int connfd) {
  if (!ckeckArg(1, "USER [user_name]")) {
    return FAIL;
  }

  if (argv[0] != "anonymous") {
    return FAIL;
  }

  return SUCC;
}

void checkArg(int argc, char format[], int connfd) {
  if (argc != argc) {
    printf("Syntax error. Input as\n\t%s", format);
    response(connfd, RC_SYNTAX);
    return false;
  } else {
    return true;
  }
}
