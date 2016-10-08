#include "command.h"

int cmd_user(int argc, char *argv[]) {
  if (!ckeckArg(1, 'USER [user_name]')) {
    return FAIL;
  }

  return SUCC;
}

void checkArg(int argc, char format[]) {
  if (argc != argc) {
    printf("Invalid arguments format. Input as\n\t%s", format);
    return false;
  } else {
    return true;
  }
}
