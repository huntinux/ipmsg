#include "net.h"

void printf_address(SOCKET fd, struct sockaddr *in_addr, socklen_t in_len, const char *msg)
{
	char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
	if (getnameinfo(in_addr, in_len, hbuf, sizeof hbuf, sbuf, sizeof sbuf, NI_NUMERICHOST | NI_NUMERICSERV) == 0)
	{
		fprintf(stdout, "[%8d]%s:  (host=%s, port=%s)", fd, msg, hbuf, sbuf);
	}
}

bool make_socket_non_blocking(SOCKET sfd)
{
	int flags;
	flags = fcntl(sfd, F_GETFL, 0);
	if (flags == -1)
	{
		fprintf(stderr, "fcntl");
		return false;
	}

	flags |= O_NONBLOCK;
	if (fcntl(sfd, F_SETFL, flags) == -1)
	{
		fprintf(stderr, "fcntl");
		return false;
	}
	flags = 1;
	if (setsockopt(sfd, IPPROTO_TCP, TCP_NODELAY, (char *)&flags, sizeof(int)) < 0)
	{
		fprintf(stderr, "setsockopt TCP_NODELAY error, errno %d", errno);
	}
	return true;
}

SOCKET create_and_bind(const char *address, const char *port)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int s;
	SOCKET sfd;

	memset(&hints, 0, sizeof (struct addrinfo));
	hints.ai_family = AF_UNSPEC;     /* Return IPv4 and IPv6 choices */
	hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
	hints.ai_flags = AI_PASSIVE;     /* All interfaces */

	s = getaddrinfo(address, port, &hints, &result);
	if (s != 0)
	{
		fprintf(stderr, "getaddrinfo: %s", gai_strerror(s));
		return INVALID_SOCKET;
	}

	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == INVALID_SOCKET)
			continue;
		int enable = 1;
		if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (char *)&enable, sizeof(int)) < 0)
		{
			fprintf(stderr, "error setsockopt SO_REUSEADDR");
		}

		s = bind(sfd, rp->ai_addr, rp->ai_addrlen);
		if (s == 0)
		{
			/* We managed to bind successfully! */
			printf_address(sfd, rp->ai_addr, rp->ai_addrlen, "Listen on");
			break;
		}
		closesocket(sfd);
	}

	if (rp == NULL)
	{
		perror("Could not bind\n");
		return INVALID_SOCKET;
	}

	freeaddrinfo(result);

	return sfd;
}
SOCKET create_and_connec(const char *address, const char *port)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int s;
	SOCKET sfd;

	memset(&hints, 0, sizeof (struct addrinfo));
	hints.ai_family = AF_UNSPEC;     /* Return IPv4 and IPv6 choices */
	hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
	hints.ai_flags = AI_PASSIVE;     /* All interfaces */

	s = getaddrinfo(address, port, &hints, &result);
	if (s != 0)
	{
		fprintf(stderr, "getaddrinfo: %s", gai_strerror(s));
		return INVALID_SOCKET;
	}

	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == INVALID_SOCKET)
			continue;

		s = connect(sfd, rp->ai_addr, rp->ai_addrlen);
		if (s == 0)
		{
			/* We managed to bind successfully! */
			printf_address(sfd, rp->ai_addr, rp->ai_addrlen, "Connect to");
			int flags = 1;
			if (setsockopt(sfd, IPPROTO_TCP, TCP_NODELAY, (char *)&flags, sizeof(int)) < 0)
			{
				fprintf(stderr, "setsockopt TCP_NODELAY error, errno %d", errno);
			}
			break;
		}
		closesocket(sfd);
	}

	if (rp == NULL)
	{
		fprintf(stderr, "Could not bind\n");
		return INVALID_SOCKET;
	}

	freeaddrinfo(result);

	return sfd;
}


