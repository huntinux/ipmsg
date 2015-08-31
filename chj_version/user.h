#ifndef _USER_H
#define _USER_H

#include <stddef.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define NAME_MAX 20
#define HOST_MAX 20

struct userlist
{
	char name[NAME_MAX];	
	char host[HOST_MAX];	
	int s_addr;		 // ip addr
	struct userlist *next;
};

int search_by_addr(int addr);
void add_user(const struct userlist *u);
void del_user(const struct userlist *u);
int get_addr_by_name(const char *name);
void show_users();
void free_userlist();

#endif	
