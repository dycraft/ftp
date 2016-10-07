#include "reply.h"

char *reply[554] = {};

void initReply() {
  reply[200] = "200 Command okay.\n";
  reply[500] = "500 Syntax error, Command unrecognized.\n";
  reply[501] = "501 Syntax error in parameters or arguments.\n";
  reply[202] = "202 Command not implemented, superfluous at this site.\n";
  reply[502] = "502 Command not implemented.\n";
  reply[503] = "503 Bad sequence of Commands.\n";
  reply[504] = "504 Command not implemented for that parameter.\n";

  reply[110] = "110 Restart marker reply.\n";
  reply[211] = "211 System status, or system help reply.\n";
  reply[212] = "212 Directory status.\n";
  reply[213] = "213 FIle status.\n";
  reply[214] = "214 Help message.\n";
  reply[215] = "215 NAME system type.\n";

  reply[120] = "120 Service ready in nnn minutes.\n";
  reply[220] = "220 Service ready for new user\n";
  reply[221] = "221 Service closing control connection.\n";
  reply[421] = "421 Service not available, closing control connection.\n";
  reply[125] = "125 Data connection already open; transfer starting.\n";
  reply[225] = "225 Data connection open; no transfer in progress.\n";
  reply[425] = "425 Can't open data connection.\n";
  reply[226] = "226 Closing data connection.\n";
  reply[426] = "427 Connection closed; transfer aborted.\n";
  reply[227] = "227 Entering Passive Mode (h1,h2,h3,h4,p1,p2).\n";

  reply[230] = "230 User logged in, proceed.\n";
  reply[530] = "530 Not logged in.\n";
  reply[331] = "331 User name okay, need password.\n";
  reply[332] = "332 Need account for login.\n";
  reply[532] = "532 Need account for storing files.\n";

  reply[150] = "150 File status okay; about to open data connection.\n";
  reply[250] = "250 Requested file action okay, completed.\n";
  reply[257] = "257 'PATHNAME' created.\n";
  reply[350] = "350 Requested file action pending further information.\n";
  reply[450] = "450 Requested file action not taken.\n";
  reply[550] = "550 Requested action not taken.\n";
  reply[451] = "451 Requested action aborted. Local error in processing.\n";
  reply[551] = "551 Requested action aborted. Page type unknown.\n";
  reply[452] = "452 Requested action not taken.\n";
  reply[552] = "552 Requested file action aborted.\n";
  reply[553] = "553 Requested action not taken.\n";
}
