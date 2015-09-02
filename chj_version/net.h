#ifndef _NET_H
#define _NET_H

#include "def.h"

void printf_address(SOCKET fd, struct sockaddr *in_addr, socklen_t in_len, const char *msg);
bool make_socket_non_blocking(SOCKET sfd);
SOCKET create_and_bind(const char *address, const char *port);
SOCKET create_and_connec(const char *address, const char *port);



int tcp_connect(const char *host, const char *serv);
int tcp_listen(const char *host, const char *serv, socklen_t *addrlenp);
int udp_server(const char *host, const char *serv, socklen_t *addrlenp);
int udp_client(const char *host, const char *serv, struct sockaddr **saptr, socklen_t *lenp);
int udp_connect(const char *host, const char *serv);

#endif
