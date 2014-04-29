/* ************************************************************************
 *       Filename:  file_manager.h
 *    Description:  
 *        Version:  1.0
 *        Created:  2010年03月08日 10时28分50秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/
#ifndef _FILE_H_
#define _FILE_H_

#define SENDFILE	0
#define RECVFILE	1

//文件信息结构体
typedef struct filelist
{
	char name[50];
	int num;
	long pkgnum;
	long size;
	long ltime;
	char user[10];	
	struct filelist *next;
}IPMSG_FILE;

//向链表(接收或发送链表)中添加一个文件
//flag : SENDFILE(发送链表) RECVFILE(接收链表)
void add_file(IPMSG_FILE temp, int flag);
//从链表(接收或发送链表)中删除一个文件
//flag : SENDFILE(发送链表) RECVFILE(接收链表)
void del_file(IPMSG_FILE *temp, int flag);
//在接收链表中按照序号(id)查找文件
IPMSG_FILE * find_file(int id);
//根据包标号和文件序号从发送链表中获取文件
IPMSG_FILE *getfileinfo(long pkgnum, int filenum);
//获取文件属性存放在fileopt中,并将文件信息存入发送链表中
int getfileopt(char *fileopt, char *filename, long pkgnum, int num);
//打印接收文件链表
void file_list(void);

//void free_link(void);

#endif	//_FILE_H_
