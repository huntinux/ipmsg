#ifndef _DEF_H
#define _DEF_H

#define TCP_NODELAY         0x0001
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#define closesocket close
#define Sleep(t) usleep(1000*(t))

typedef enum{false=0,true=!false} bool ;
typedef int SOCKET;

#endif
