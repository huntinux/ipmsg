/* ************************************************************************
 *       Filename:  user_manager.h
 *    Description:  
 *        Version:  1.0
 *        Created:  2010年03月08日 10时28分50秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/

#ifndef _LIST_H_
#define _LIST_H_

//用户信息结构体
typedef struct userlist
{
	char name[20];	//用户名
	char host[20];	//主机名
	int s_addr;		//IP地址(32位网络字节序)
	struct userlist *next;
}IPMSG_USER;

//向用户列表中添加一个用户节点
void add_user(IPMSG_USER temp);
//从用户列表中删除一个用户节点
void del_user(IPMSG_USER temp);
//根据用户名返回IP地址
int get_addr_by_name(char *name);
//打印用户列表
void list(void);
//void free_link(void);

#endif	//_LIST_H_
