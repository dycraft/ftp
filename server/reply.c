#include "reply.h"

char *reply[554] = {};

void reply_init() {
  reply[200] = "200 Command okay.\r\n";
  reply[500] = "500 Syntax error, Command unrecognized.\r\n";
  reply[501] = "501 Syntax error in parameters or arguments.\r\n";
  reply[202] = "202 Command not implemented, superfluous at this site.\r\n";
  reply[502] = "502 Command not implemented.\r\n";
  reply[503] = "503 Bad sequence of Commands.\r\n";
  reply[504] = "504 Command not implemented for that parameter.\r\n";

  reply[110] = "110 Restart marker reply.\r\n";
  reply[211] = "211 System status, or system help reply.\r\n";
  reply[212] = "212 Directory status.\r\n";
  reply[213] = "213 FIle status.\r\n";
  reply[214] = "214 Help message.\r\n";
  reply[215] = "215 NAME system type.\r\n";

  reply[120] = "120 Service ready in nnn minutes.\r\n";
  reply[220] = "220 Service ready for new user\r\n";
  reply[221] = "221 Service closing control connection.\r\n";
  reply[421] = "421 Service not available, closing control connection.\r\n";
  reply[125] = "125 Data connection already open; transfer starting.\r\n";
  reply[225] = "225 Data connection open; no transfer in progress.\r\n";
  reply[425] = "425 Can't open data connection.\r\n";
  reply[226] = "226 Closing data connection.\r\n";
  reply[426] = "427 Connection closed; transfer aborted.\r\n";
  reply[227] = "227 Entering Passive Mode (h1,h2,h3,h4,p1,p2).\r\n";

  reply[230] = "230 User logged in, proceed.\r\n";
  reply[530] = "530 Not logged in.\r\n";
  reply[331] = "331 User name okay, need password.\r\n";
  reply[332] = "332 Need account for login.\r\n";
  reply[532] = "532 Need account for storing files.\r\n";

  reply[150] = "150 File status okay; about to open data connection.\r\n";
  reply[250] = "250 Requested file action okay, completed.\r\n";
  reply[257] = "257 'PATHNAME' created.\r\n";
  reply[350] = "350 Requested file action pending further information.\r\n";
  reply[450] = "450 Requested file action not taken.\r\n";
  reply[550] = "550 Requested action not taken.\r\n";
  reply[451] = "451 Requested action aborted. Local error in processing.\r\n";
  reply[551] = "551 Requested action aborted. Page type unknown.\r\n";
  reply[452] = "452 Requested action not taken.\r\n";
  reply[552] = "552 Requested file action aborted.\r\n";
  reply[553] = "553 Requested action not taken.\r\n";
}


// send() with reply code
int response(int sockfd, int rc) {
  int rc_n = htonl(rc);
  printf("Send: %s", reply[rc]);

  if (send(sockfd, &rc_n, sizeof(rc_n), 0) == -1) {
    printf("Error send(): %s(%d)\r\n", strerror(errno), errno);
    return FAIL;
  }
  return SUCC;
}
