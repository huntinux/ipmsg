/* ************************************************************************
 * Filename: user_interface.c
 * Description:
 * Version: 1.0
 * Created: 2010年03月08日 10时28分50秒
 * Revision: none
 * Compiler: gcc
 * Author: YOUR NAME (),
 * Company:
 * ************************************************************************/
#include "myinclude.h"
#include "user_manager.h"
#include "file_manager.h"
#include "user_interface.h"
#include "communication.h"
//帮助菜单
char *help= "*********************************************************************************\n"\
			 "* send/say [username] : 发送消息*\n"\
			 "* sendfile [username] [filename...] : 发送文件*\n"\
			 "* getfile [filenum] : 接收文件*\n"\
			 "* list/ls : 打印用户列表*\n"\
			 "* list/ls file : 打印接收文件列表*\n"\
			 "* clear/cls : 清屏*\n"\
			 "* help ： 帮助*\n"\
			 "* exit/quit : 推出*\n"\
			 "**********************************************************************************\n"
			 ;
			 //函数指针
			 typedef void (*FUN)(int argc, char *argv[]);

			 //命令结构体
			 typedef struct cmd
{
	char *name;	//命令名称
	FUN fun;	//命令处理函数
}CMD;


//发送命令
//send [user] [msg]
//sendfile [user] [file1] [file2] ...
void send_fun(int argc, char *argv[])
{
	struct sockaddr_in addr = {AF_INET};
	int s_addr = 0;
	char name[20] = "";
	char buf[100] = "";	

	char sendbuf[200]="";
	char temp[100]="";
	int templen = 0;
	long t = time((time_t*)NULL);
	int len = sprintf(sendbuf, "1:%ld:%s:%s", t, user(), host());

	if(argc < 2)	//参数小于2，则需输入用户名
	{
		list();
		IPMSG_OUT_MSG_COLOR(
					printf("please input user name:");
					)
			fgets(name, sizeof(name), stdin);
		name[strlen(name)-1] = 0;
		argv[1] = name;
	}

	s_addr = get_addr_by_name(argv[1]);
	if(s_addr == 0)
	{
		printf("error: no this user:\"%s\"!\n", argv[1]);
		return ;
	}
	if(argc < 3)	//参数小于2，则需输入消息或文件名
	{
		if(strcmp(argv[0], "sendfile")==0)
		{
			IPMSG_OUT_MSG_COLOR(	
						printf("please input filename:");
						)
		}
		else
		{
			IPMSG_OUT_MSG_COLOR(	
						printf("please input message:");
						)
		}
		fgets(buf, sizeof(buf), stdin);
		buf[strlen(buf)-1] = 0;
		argv[2] = buf;

		//如果是发送文件则需切割输入的多个文件名
		if( argv[0][4]=='f' )	
		{
			int i = 2;
			argv[i] = buf;
			while((argv[i]=strtok(argv[i], " \t"))!=NULL) i++;
		}
	}	

	if(strcmp(argv[0], "sendfile")==0)
	{
		int i = 2;
		char *fileopt = NULL;
		templen = sprintf(temp, ":%ld:%c",IPMSG_SENDMSG|IPMSG_SENDCHECKOPT|IPMSG_FILEATTACHOPT,0);
		fileopt = temp+templen;
		//添加多个文件属性
		while(argv[i]!=NULL)
		{
			len = getfileopt(fileopt, argv[i], t, i-2);
			templen += len;
			fileopt += len;
			i++;
		}
	}
	else
	{
		templen = sprintf(temp, ":%ld:%s", IPMSG_SENDMSG|IPMSG_SENDCHECKOPT, argv[2]);
	}

	memcpy(sendbuf+len, temp, templen);

	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = s_addr;
	msg_send(sendbuf,len+templen, addr);
}

//接收文件
//命令格式: getfile num
// num : 文件在链表中的序号，可用“ls file”命令查看
void getfile_fun(int argc, char *argv[])
{
	int id = 0;
	if( argc!=2 )
	{
		IPMSG_OUT_MSG_COLOR(
					printf("error cmd param\n");
					printf("command: getfile num\n");
					)
			return ;
	}
	if((id=atoi(argv[1]))<0)
	{
		IPMSG_OUT_MSG_COLOR(
					printf("No such file!\n");
					)
			return ;
	}
	recvfile(id);
}

//打印用户或文件列表
//ls : 打印用户列表
//ls file : 打印接收文件列表
void list_fun(int argc, char *argv[])
{
	if( argv[1]==NULL )
	  list();	//用户列表
	else if(strcmp("file", argv[1])==0)
	  file_list();	//文件列表
	else
	  printf("command not find!\n");
}

// 帮助命令
void help_fun(int argc, char *argv[])
{
	//打印帮助菜单
	IPMSG_OUT_MSG_COLOR(
				printf("%s", help);	
				)
}

//退出命令
void exit_fun(int argc, char *argv[])
{
	ipmsg_exit();
	// free_link();
	exit(1);
}

//清屏命令
void clear_fun(int argc, char *argv[])
{
	write(1, "\033[2J\033[0;0H", 10);
}

//命令数组：用来保存 命令名 和 处理函数名
CMD cmdlist[]={	
	{"send", send_fun},
	{"say", send_fun},	
	{"sendfile", send_fun},	
	{"getfile", getfile_fun},
	{"list", list_fun},	
	{"ls", list_fun},	
	{"help", help_fun},
	{"exit", exit_fun},
	{"quit", exit_fun},
	{"clear", clear_fun},	
	{"cls", clear_fun}	
};

//分析、分配命令
int exec_cmd(char *cmd)
{
	char *argv[10] = {NULL};
	int argc = 0;
	int i = 0;
	if(strlen(cmd)==0)
	  return 0;

	argv[0] = cmd;
	while((argv[argc]=strtok(argv[argc], " \t"))!=NULL) argc++;

	/*add by wenhao*/
	if(argc == 0){
		return -1;
	}
	/*end by wenhao*/

	for (i=0;i<sizeof(cmdlist)/sizeof(CMD);i++)
	{
		//查找命令
		if (strcmp(cmdlist[i].name, argv[0])==0)
		{
			//执行命令
			cmdlist[i].fun(argc, argv);
			return 0;
		}
	}	
	return -1;
}	

void *user_interface(void *arg)
{
	write(1, "\033[32m", 5);
	while(1)
	{
		char buf[100]="";
		write(1,"\rIPMSG:",7);
		fgets(buf, sizeof(buf), stdin);
		buf[strlen(buf)-1]=0;
		if(exec_cmd(buf) < 0)
		{
			IPMSG_OUT_MSG_COLOR(
						printf("command not find!\n");
						)
		}
	}
	return NULL;
}
