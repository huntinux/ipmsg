/* ************************************************************************
 *       Filename:  user_manager.c
 *    Description:  
 *        Version:  1.0
 *        Created:  2010年03月08日 10时28分50秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/

#include "myinclude.h"
#include "user_manager.h"

static IPMSG_USER *head = NULL;

//向用户列表中添加一个用户节点
void add_user(IPMSG_USER temp)
{
	IPMSG_USER *pf=head, *pb=NULL, *pend=NULL;
	
	while(pf != NULL)
	{
		if(pf->s_addr == temp.s_addr)	//用户是否已经在链表中
			return;
		pend = pf;
		pf = pf->next;	
	}
	pb=(IPMSG_USER *)malloc(sizeof(IPMSG_USER));
	strcpy(pb->name, temp.name);
	strcpy(pb->host, temp.host);
	pb->s_addr = temp.s_addr;	
	pb->next=NULL;
	
	if(head == NULL)
		head = pb;
	else
		pend->next = pb;
}

//从用户列表中删除一个用户节点
void del_user(IPMSG_USER user)
{
	IPMSG_USER *p1=head,*p2=NULL;
	if(head==NULL)
		return ;
	else
	{
		while((strcmp(p1->name,user.name)!=0)&&(p1->next!=NULL))
		{
			p2=p1;
			p1=p1->next;
		}
		if(strcmp(p1->name,user.name)==0)
		{
			if(p1==head)
				head=p1->next;
			else 
				p2->next=p1->next;
			free(p1);
		}
	}
}

//根据用户名返回IP地址
int get_addr_by_name(char *name)
{
	IPMSG_USER *p=head;
	while(p!=NULL)
	{
		if(strcmp(p->name, name)==0)
		{
			return p->s_addr;
		}
		p = p->next;
	}	
	return 0;
}

//打印用户列表
void list(void)
{
	IPMSG_USER *p=head;
	IPMSG_OUT_MSG_COLOR(
	printf("%10s\t%10s\t%s\n", "Username","Hostname","IP Address");
	)
	while(p!=NULL)
	{
		struct in_addr addr = {p->s_addr};
		IPMSG_OUT_MSG_COLOR(
		printf("%10s\t%10s\t%s\n",p->name,p->host,inet_ntoa(addr));
		)
		p=p->next;
	}	
}
/*
void free_link(void)
{
	IPMSG_USER *pf=head, *p=head;	
	while(p!=NULL)
	{
			p=p->next;
			free(pf);
			pf = p;
	}
}
*/
