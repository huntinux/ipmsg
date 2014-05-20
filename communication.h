/* ************************************************************************
* Filename: communicatioh.h
* Description:
* Version: 1.0
* Created: 2010年03月08日 10时28分50秒
* Revision: none
* Compiler: gcc
* Author: YOUR NAME (),
* Company:
* ************************************************************************/
#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_

#include "myinclude.h"
#define PORT 2425
#define BUFF_MAX 256
#define LISTEN_MAX 10

//接收消息线程，接收其他客户端发送的UDP数据
void *recv_msg_thread(void *arg);
//发送文件线程，等待其他客户端接收文件并负责向其传送文件
void *sendfile_thread(void *arg);

//上线
void online(const char *user, const char *host);
//下线
void ipmsg_exit(void);

//发送信息
void msg_send(char *msg, int len, struct sockaddr_in addr);
//接收文件(参数为接收文件列表中的序号)
int recvfile(int id);

//获取用户名
char *user(void);
//获取主机名
char *host(void);
//获取UDP描述符
int udp_fd(void);

#endif //_COMMUNICATION_H_
