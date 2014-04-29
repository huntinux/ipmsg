#ifndef _MYINCLUDE_H_
#define _MYINCLUDE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include "ipmsg.h"

#define IPMSG_OUT_MSG_COLOR(x)   { \
write(1, "\033[31m", 5);\
x \
write(1, "\033[32m", 5);}

#endif//_MYINCLUDE_H_

