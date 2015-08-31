#ifndef _NET_H
#define _NET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include "def.h"

void printf_address(SOCKET fd, struct sockaddr *in_addr, socklen_t in_len, const char *msg);
bool make_socket_non_blocking(SOCKET sfd);
SOCKET create_and_bind(const char *address, const char *port);
SOCKET create_and_connec(const char *address, const char *port);

#endif
