#ifndef _DEF_H
#define _DEF_H

#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netdb.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<stdarg.h>		/* ANSI C header file */
#include	<syslog.h>		/* for syslog() */
#include	<errno.h>
#include	<string.h>
#include	<malloc.h>

#include	"error.h"


#define TCP_NODELAY         0x0001
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#define closesocket close
#define Sleep(t) usleep(1000*(t))
#define MAXLINE 1024
#define LISTENQ 256

typedef enum{false=0,true=!false} bool ;
typedef int SOCKET;

#endif
