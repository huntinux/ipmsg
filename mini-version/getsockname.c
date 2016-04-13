#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>

#define INVALID_SOCKET (-1)

static void printf_address(int fd, struct sockaddr *in_addr, socklen_t in_len, const char *msg)
{
    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
    if (getnameinfo(in_addr, in_len, hbuf, sizeof hbuf, sbuf, sizeof sbuf, NI_NUMERICHOST | NI_NUMERICSERV) == 0)
    {
        printf("[%8d]%s:  (host=%s, port=%s)", fd, msg, hbuf, sbuf);
    }
}

int main()
{
    int udpfd = INVALID_SOCKET;
    if((udpfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket UDP");
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9090);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(udpfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("Bind UDP");
        return -2;
    }

    struct sockaddr_in addr_auto;
    socklen_t addr_auto_len = sizeof(addr_auto);
    memset(&addr_auto, 0, sizeof(struct sockaddr_in));
    if(getsockname(udpfd, &addr_auto, &addr_auto_len) == -1)
    {
        perror("getsockname");
        return -3;
    }

    printf_address(udpfd, (struct sockaddr *)&addr_auto, addr_auto_len, "Address auto");
    return 0;

}
