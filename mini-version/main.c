#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <libgen.h>
#include <netdb.h>
#include <time.h>
#include <poll.h>
#include <ifaddrs.h>
#include <signal.h>
#include "ipmsg.h"

#define buf_MAX 1024
#define FDNUM 1
#define INVALID_SOCKET -1

const char *user_name = "default";
const char *host_name = "default";

static int islocaladdr(struct in_addr addr)
{
    struct ifaddrs *ifaddr, *ifa;
    int family, n, ret = 0;

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return -1;
    }

    /* Walk through linked list, maintaining head pointer so we
       can free list later */
    for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {
        if (ifa->ifa_addr == NULL)
            continue;

        family = ifa->ifa_addr->sa_family;
        if(family != AF_INET)
            continue;

        /* For an AF_INET interface address , check if addr is in them*/
        struct sockaddr_in *sai = (struct sockaddr_in *)ifa->ifa_addr;
        if(sai->sin_addr.s_addr == addr.s_addr)
        {
            /* Find addr in local addresses */
            return 1;
        }
    }

    if(ifa == NULL)
        ret = 0; /* Not Found */

    freeifaddrs(ifaddr);
    return ret;
}

static void printf_address(int fd, struct sockaddr *in_addr, socklen_t in_len, const char *msg)
{
    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
    if (getnameinfo(in_addr, in_len, hbuf, sizeof hbuf, sbuf, sizeof sbuf, NI_NUMERICHOST | NI_NUMERICSERV) == 0)
    {
        printf("[%8d]%s:  (host=%s, port=%s)\n", fd, msg, hbuf, sbuf);
    }
}

static int create_and_bind(const char *port)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int s;
    const char *address = NULL;
    int sfd;

    memset(&hints, 0, sizeof (struct addrinfo));
    hints.ai_family = AF_UNSPEC;     /* Return IPv4 and IPv6 choices */
    hints.ai_socktype = SOCK_DGRAM; /* We want a UDP socket */
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

        /* Make broadcast */
        int enable = 1;
        if(setsockopt(sfd, SOL_SOCKET, SO_BROADCAST, &enable, sizeof(int)) < 0)
        {
            fprintf(stderr, "error setsockopt SO_REUSEADDR");
        }

        s = bind(sfd, rp->ai_addr, rp->ai_addrlen);
        if (s == 0)
        {
            /* We managed to bind successfully! */
            printf_address(sfd, rp->ai_addr, rp->ai_addrlen, "UDP bind to");
            break;
        }
        close(sfd);
    }

    if (rp == NULL)
    {
        fprintf(stderr, "Could not bind\n");
        sfd = INVALID_SOCKET;
    }

    freeaddrinfo(result);

    return sfd;
}

void test_send(int udpfd, const char *port)
{
    char buf[100]="";
    int t = time((time_t *)NULL);
    int len = sprintf(buf,"1:%d:%s:%s:%ld:%s", \
            t,user_name,host_name,IPMSG_BR_EXIT,"Message from X1 Carbon");

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(atoi(port));
    inet_pton(AF_INET, "192.168.5.252", &addr.sin_addr.s_addr);
    sendto(udpfd, buf, len, 0, (struct sockaddr*)&addr,sizeof(addr));	
}


void ipmsg_exit(int udpfd, const char *port)
{
    char buf[100]="";
    int t = time((time_t *)NULL);
    int len = sprintf(buf,"1:%d:%s:%s:%ld:%s", \
            t,user_name,host_name,IPMSG_BR_EXIT,user_name);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(atoi(port));
    addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);	
    sendto(udpfd, buf, len, 0, (struct sockaddr*)&addr,sizeof(addr));	
}

int g_prepare_exit = 0;
static void SigHandler(int sig, siginfo_t *siginfo, void *ignore){
    switch (sig)
    {
        case SIGINT:
            /* broadcast offline */
            printf("Caught SIGINT! Shutdown\n");
            g_prepare_exit = 1;
            break;
        case SIGUSR1:
            break;
        case SIGUSR2:
            break;
        default:
            break;
    }
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        fprintf(stderr, "Usage: %s [port]\n", basename(argv[0]));
        return -1;
    }

    const char *port = argv[1];
    int udpfd = create_and_bind(port);
    if(udpfd < 0) return -1;

    /* broadcast */
    char buf[buf_MAX];
    memset(buf, '\0', buf_MAX);
    int t = time((time_t *)NULL);
    int len = sprintf(buf,"1:%d:%s:%s:%ld:%s", \
            t,user_name,host_name,IPMSG_BR_ENTRY,user_name);

    struct sockaddr_in broadcast_addr;
    memset(&broadcast_addr, 0, sizeof(struct sockaddr_in));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(atoi(port));
    broadcast_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    int ns = sendto(udpfd, buf, len, 0, (struct sockaddr*)&broadcast_addr,sizeof(broadcast_addr));	
    if(ns  == -1)
    {
        perror("sendto");
        return -4;
    }

    /* signal handle */
    struct sigaction sigact;
    memset(&sigact, 0, sizeof(sigact));
    sigact.sa_flags = SA_SIGINFO;
    sigact.sa_sigaction = SigHandler;
    sigaction(SIGINT, &sigact, NULL);

    /* add updfd to pollfd, start poll thread for waiting I/O on udpfd */
    struct pollfd pfd[FDNUM];
    pfd[0].fd = udpfd;
    pfd[0].events = POLLIN | POLLOUT;
    pfd[0].revents = 0;
    while(1)
    {
        memset( buf, '\0', sizeof( buf ) );
        int n = poll(pfd, FDNUM, -1);
        int i;
        for(i = 0; i < n; i++)
        {
            int sfd = pfd[i].fd;
            if(pfd[i].revents & POLLIN)
            {
                struct sockaddr_in peer_addr;
                socklen_t peer_len = sizeof(peer_addr);
                int nr = recvfrom(sfd, buf, sizeof(buf), 0, (struct sockaddr *)&peer_addr, &peer_len);
                if(nr > 0)
                {
                    printf("get %d bytes data: %s\n", nr, buf);

                    /* parse */
                    int t; 
                    unsigned long cmd = 0;
                    char ver[256],username[256], hostname[256], other[256];
                    sscanf(buf, "%[^:]:%d:%[^:]:%[^:]:%lu:%s", ver, &t, username, hostname, &cmd, other);
                    char * extra_msg = strrchr(buf, ':');
                    if(extra_msg) 
                        printf("Get extra message: %s\n", extra_msg);

                    printf_address(udpfd, (struct sockaddr *)&peer_addr, peer_len, "Peer addr");
                    switch(GET_MODE(cmd))
                    {
                        case IPMSG_BR_ENTRY:
                            if(islocaladdr(peer_addr.sin_addr))
                            {
                                fprintf(stderr, "local addr, ignore the data\n");
                                continue; 
                            }

                            /* Todo: add user */
                            printf("find new client: %s-%s\n", username, hostname);

                            /* answer entrance */
                            t = time((time_t *)NULL);
                            len = sprintf(buf,"1:%d:%s:%s:%lu:%s", t, user_name, host_name, IPMSG_ANSENTRY, user_name);
                            sendto(udpfd, buf, len, 0, (struct sockaddr*)&peer_addr, peer_len);
                            break;

                        case IPMSG_ANSENTRY:
                            /* Todo: add user */
                            printf("find new client: %s-%s\n", username, hostname);
                            break;

                        case IPMSG_SENDMSG:
                            /* other client send message to us */
                            if(extra_msg && (cmd & IPMSG_SENDCHECKOPT))
                            {
                                /* give a ack */ 
                	            char buf[50]="";
					            int ct = time((time_t *)NULL);
					            int len = sprintf(buf,"1:%d:%s:%s:%ld:%d",ct,user_name,host_name,IPMSG_RECVMSG, t);
					            sendto(udpfd, buf, len, 0, (struct sockaddr*)&peer_addr, sizeof(peer_addr));
                            }
                        default:
                            break;
                        }
                    }
                    else
                    {
                        /* error occures */
                        perror("recvfrom");
                        break;
                    }
            }
            else if(pfd[i].revents & POLLOUT)
            {
                /* broadcast exit */
                if(g_prepare_exit)
                {
                    ipmsg_exit(pfd[i].fd, port);
                    g_prepare_exit = 0;
                    exit(1);
                }

                /* Test for send message */
                test_send(udpfd, port);
            }
            else
            {
                fprintf(stderr, "otherthing occured...\n");
            }
        }

    }


    close(udpfd);
    return 0;

}
