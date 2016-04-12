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
#include "ipmsg.h"


/*
   const char * menu = " \n
ls      :list users \n
send    :user_ip message \n
quit    \n
"
*/

#define PORT 8989 
#define buf_MAX 1024
#define FDNUM 1

const char *user_name = "client8989";
const char *host_name = "host8989";

int main()
{
    /* prepare UDP socket for receiving message from other clients */
    int udpfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(udpfd < 0) 
    {
        perror("Socket UDP");
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if( bind(udpfd, (struct sockaddr*)&addr, sizeof(addr)) < 0 )
    {
        perror("Bind UDP");
        return -2;
    }

    /* make udp broadcast */
    int broadcast = 1;
    setsockopt(udpfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(int));

    /* broadcast */
    char buf[buf_MAX];
    memset(buf, '\0', buf_MAX);
    int t = time((time_t *)NULL);
    int len = sprintf(buf,"1:%d:%s:%s:%ld:%s", \
            t,user_name,host_name,IPMSG_BR_ENTRY,user_name);

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9090);
    addr.sin_addr.s_addr=inet_addr("255.255.255.255");
    sendto(udpfd, buf, len, 0, (struct sockaddr*)&addr,sizeof(addr));	

    /* add updfd to pollfd, start poll thread for waiting I/O on udpfd */
    struct pollfd pfd[FDNUM];
    pfd[0].fd = udpfd;
    pfd[0].events = POLLIN | POLLOUT;
    pfd[0].revents = 0;
    while(1)
    {
        memset( buf, '\0', sizeof( buf ) );
        int n = poll(pfd, FDNUM, -1);
        for(int i = 0; i < n; i++)
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
                    
                    if( cmd == IPMSG_BR_ENTRY )
                    {
                        printf("find new client: %s-%s\n", username, hostname);
                        /* answer entrance */
                        t = time((time_t *)NULL);
                        len = sprintf(buf,"1:%d:%s:%s:%lu:%s", t, user_name, host_name, IPMSG_ANSENTRY, user_name);
                        sendto(udpfd, buf, len, 0, (struct sockaddr*)&peer_addr, peer_len);
                    }
                    else if( cmd == IPMSG_ANSENTRY)
                    {
                        printf("find new client: %s-%s\n", username, hostname);
                    }
                    else
                    {
                    }
                }
                else
                {
                    /* error occures */
                    perror("read");
                    break;
                }
            }
            else if(pfd[i].revents & POLLOUT)
            {

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
