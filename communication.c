/* ************************************************************************
 * Filename: communicatioh.c
 * Description:
 * Version: 1.0
 * Created: 2010年03月08日 10时28分50秒
 * Revision: none
 * Compiler: gcc
 * Author: YOUR NAME (),
 * Company:
 * ************************************************************************/
#include "myinclude.h"
#include "communication.h"
#include "user_manager.h"
#include "file_manager.h"
#include "lib.h"

static int udpfd = 0;
static int tcpfd = 0;
static char user_name[BUFF_MAX] = "";
static char host_name[BUFF_MAX] = "";

//创建UDP 和 TCP_Server 套接口
void 
create_server()
{
	int broadcast = 1;
	struct sockaddr_in addr;

	// Create TCP socket for SendFile Server
	tcpfd = socket(AF_INET,SOCK_STREAM,0);
	if( tcpfd < 0 )
		handle_error("Socket TCP");

	// bind addr to tcp socket
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if( bind(tcpfd, (struct sockaddr*)&addr, sizeof(addr)) < 0 )
		handle_error("Bind UDP");

	// listen
	listen(tcpfd, LISTEN_MAX);

	// Create UDP socket for commucation
	udpfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(udpfd < 0)
		handle_error("Socket UDP");

	// bind addr to udp socket
	if( bind(udpfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
		 handle_error("UDP bind");

	// 让udp能发送广播数据
	setsockopt(udpfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(int));
}

//上线广播
void broad_cast_online_info(void)
{
	char buf[BUFF_MAX]="";
	struct sockaddr_in addr = {AF_INET};
	int t = time((time_t *)NULL);
	int len = sprintf(buf,"1:%d:%s:%s:%ld:%s", \
				t,user_name,host_name,IPMSG_BR_ENTRY,user_name);

	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr=inet_addr("255.255.255.255");
	//addr.sin_addr.s_addr = htonl(-1);	//modified by wangyanjun in 10/7/9
	sendto(udpfd, buf, len, 0, (struct sockaddr*)&addr,sizeof(addr));	
}

int tcp_fd(void)
{
	return tcpfd;
}

int udp_fd(void)
{
	return udpfd;
}

char *user(void)
{
	return user_name;
}

char *host(void)
{
	return host_name;
}

//下线广播
void ipmsg_exit(void)
{
	char buf[100]="";
	struct sockaddr_in addr = {AF_INET};
	int t = time((time_t *)NULL);
	int len = sprintf(buf,"1:%d:%s:%s:%ld:%s", \
				t,user_name,host_name,IPMSG_BR_EXIT,user_name);
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr=inet_addr("255.255.255.255");	
	sendto(udpfd, buf, len, 0, (struct sockaddr*)&addr,sizeof(addr));	
}

//上线并初始化系统
void 
online(const char *user, const char *host)
{
	//strcpy(user_name,user);
	snprintf(user_name, sizeof(user_name), "%s", user);
	//strcpy(host_name,host);
	snprintf(host_name, sizeof(host_name), "%s", host);
	create_server();
	broad_cast_online_info();
}

//发送消息
void msg_send(char *msg, int len, struct sockaddr_in addr)
{
	sendto(udpfd, msg, len, 0, (struct sockaddr*)&addr, sizeof(addr));
}

//接收文件(参数为接收文件列表中的序号)
int recvfile(int id)
{
	int fd = 0;
	char buf[2048]="";
	FILE *fp = NULL;
	unsigned long len = 0;
	struct sockaddr_in addr = {AF_INET};
	int s_addr = 0;
	IPMSG_FILE *p = find_file(id);	//是否存在该文件
	if( p==NULL	)
	{
		IPMSG_OUT_MSG_COLOR(
					printf("no such file id\n");
					)
			return -1;
	}

	s_addr = get_addr_by_name(p->user);	//根据发送者姓名获取发送这地址
	if( s_addr == 0 )
	{
		IPMSG_OUT_MSG_COLOR(
					printf("recv file error: user is not online!\n");
					)
			del_file(p, RECVFILE);
		return -1;
	}

	fd = socket(AF_INET, SOCK_STREAM, 0);	//创建临时TCP client用来接收文件
	if( fd < 0 )
	{
		IPMSG_OUT_MSG_COLOR(
					printf("recv file error: creat socket error!\n");
					)
			return -1;
	}
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = s_addr;
	if(connect(fd, (struct sockaddr*)&addr, sizeof(addr))!=0)
	{
		perror("recvfile connect");
		return -1;
	}
	len = sprintf(buf, "1:%ld:%s:%s:%ld:%lx:%d:0", time((time_t*)NULL),\
				user(), host(), IPMSG_GETFILEDATA, p->pkgnum, p->num);
	send(fd, buf, len, 0);	//发送IPMSG_GETFILEDATA
	fp = fopen(p->name, "w");
	if( fp==NULL )
	{
		perror("savefile");
		return -1;
	}
	len = 0;
	do	//接收文件
	{
		int rlen = recv(fd, buf, sizeof(buf), 0);
		len += rlen;
		IPMSG_OUT_MSG_COLOR(
					printf("\rrecvlen=%d%%", (int)((100*len)/p->size));

					)
			fflush(stdout);
		fwrite(buf, 1, rlen, fp);
	}while(len < p->size);

	printf("\n");
	close(fd);	//关闭TCP Client
	fclose(fp); //关闭文件
	del_file(p, RECVFILE);	//从文件列表中删除接收过的文件
	return 0;
}

//发送文件的线程
void *sendfile_thread(void *arg)
{
	int fd = tcp_fd();	//获取TCP_Server套接口描述符
	while(1)
	{
		struct sockaddr_in addr = {AF_INET};
		unsigned int addrlen = sizeof(addr);
		int clifd = accept(fd, (struct sockaddr*)&addr, &addrlen);
		if( clifd<0 )
		{
			perror("accept");
			exit(1);
		}
		while(1)	// 发送多个文件
		{
			IPMSG_FILE *p = NULL;
			FILE *fp = NULL;
			IPMSG_USER temp;
			long pkgnum = 0 ;
			char edition[100]="";
			long oldpkgnum = 0 ;
			long cmd = 0;
			int filenum = 0;
			char buf[1400]="";
			int sendsize = 0;	
			//接收IPMSG_GETFILEDATA
			if(recv(clifd, buf, sizeof(buf), 0)==0)
			  break;
			sscanf(buf, "%[^:]:%ld:%[^:]:%[^:]:%ld:%lx:%x",edition, &pkgnum, temp.name, temp.host, &cmd,\
						&oldpkgnum, &filenum);
			//是否是IPMSG_GETFILEDATA
			if((GET_MODE(cmd)&IPMSG_GETFILEDATA)!=IPMSG_GETFILEDATA)
			  break;
			//获取之前发送的文件信息
			if ((p = getfileinfo(oldpkgnum, filenum))==NULL)
			{
				return NULL;
			}
			if( (fp=fopen(p->name, "r"))==NULL )
			{
				IPMSG_OUT_MSG_COLOR(
							printf("senderror: no such file: %s\n", p->name);
							)
					return NULL;
			}	
			do	//发送文件
			{
				int size = fread(buf, 1, sizeof(buf), fp);
				send(clifd, buf, size, 0);
				sendsize += size;
			}while(sendsize < p->size);
			fclose(fp);	//关闭文件
			del_file(p, SENDFILE);	//从发送文件链表中删除文件
		}//end wile1 // 循环发送多个文件
		close(clifd);	//关闭套接口等待下个用户连接
	}//end while
	return NULL;
}

//接收消息线程，接收其他客户端发送的UDP数据
void *recv_msg_thread(void *arg)
{
	while(1)
	{
		char buf[500]="";	
		char edition[100]="";
		struct sockaddr_in addr = {AF_INET};
		unsigned int addrlen = sizeof(addr);
		int len = 0;
		long pkgnum = 0;
		long cmd = 0;
		char msg[100]="";
		int t = 0;
		char *p = NULL;
		IPMSG_USER temp;	
		len = recvfrom(udp_fd(), buf, sizeof(buf), 0, (struct sockaddr*)&addr, &addrlen);
		sscanf(buf, "%[^:]:%ld:%[^:]:%[^:]:%ld",edition, &pkgnum, temp.name, temp.host, &cmd);

		p = strrchr(buf, ':');	//查找附加信息
		memcpy(msg, p+1, len-(p-buf));	//将附加信息放入msg中

		temp.s_addr = addr.sin_addr.s_addr;
		switch(GET_MODE(cmd))
		{
			case IPMSG_BR_ENTRY:
				t = time((time_t *)NULL);
				len = sprintf(buf,"1:%d:%s:%s:%ld:%s",t,user(),host(),IPMSG_ANSENTRY,user());
				sendto(udp_fd(),buf,len,0,(struct sockaddr*)&addr,sizeof(addr));
			case IPMSG_ANSENTRY:
				add_user(temp);
				break;
			case IPMSG_SENDMSG:
				if(msg[0]!=0)
				{
					IPMSG_OUT_MSG_COLOR(
								printf("\r[recv msg from: %s ]#\n%s\n", temp.name, msg);
								)
						write(1,"\rIPMSG:",7);
				}
				if((cmd&IPMSG_SENDCHECKOPT)==IPMSG_SENDCHECKOPT)
				{
					char buf[50]="";
					t = time((time_t *)NULL);
					int len = sprintf(buf,"1:%d:%s:%s:%ld:%ld",t,user(),host(),IPMSG_RECVMSG, pkgnum);
					sendto(udp_fd(),buf,len,0,(struct sockaddr*)&addr,sizeof(addr));
				}
				if((cmd&IPMSG_FILEATTACHOPT)==IPMSG_FILEATTACHOPT)
				{
					char *p = msg+strlen(msg)+1;
					//printf("filemsg=%s\n",p);
					char *fileopt= strtok(p, "\a");	//fileopt指向第一个文件属性
					do{	//循环提取文件信息
						IPMSG_FILE ftemp;
						sscanf(fileopt, "%d:%[^:]:%lx:%lx", &ftemp.num, ftemp.name, &ftemp.size, &ftemp.ltime);	
						strcpy(ftemp.user, temp.name);
						ftemp.pkgnum = pkgnum;
						add_file(ftemp, RECVFILE);
						fileopt = strtok(NULL, "\a");	//fileopt指向下一个文件属性
					}while(fileopt!=NULL);
					IPMSG_OUT_MSG_COLOR(
								printf("\r<<<Recv file from %s!>>>\n", temp.name);
								)
						write(1,"\rIPMSG:",7);
				}
				break;
			case IPMSG_RECVMSG:
				{
					IPMSG_OUT_MSG_COLOR(
								printf("\r%s have receved your msg!\n", temp.name);
								)
						write(1,"\rIPMSG:",7);
				}
				break;	
			case IPMSG_BR_EXIT:
				del_user(temp);
				break;
			default :
				break;	
		}
	}
	return NULL;
}
