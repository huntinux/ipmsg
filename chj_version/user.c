#include "user.h"

static struct userlist *head = NULL;

int search_by_addr(int addr)
{
	const struct userlist *p;

	p = (const struct userlist *)head;
	while(p){
		if (p->s_addr == addr)
			return 1; // find one
		else
			p = p->next;
	}
	return 0;
}

void add_user(const struct userlist *u)
{
	struct userlist *new_user=NULL; 
	
	if (search_by_addr(u->s_addr)) {
		fprintf(stderr, "ip is used.\n");
		return;
	}

	new_user = (struct userlist *)calloc(1, sizeof(struct userlist));
	if (!new_user) {
		perror("calloc");
		return;
	}
	
	strcpy(new_user->name, u->name);
	strcpy(new_user->host, u->host);
	new_user->s_addr = u->s_addr;	
	new_user->next = NULL;
	
	if(head == NULL)
		head = new_user;
	else{
		new_user->next = head->next;
		head->next = new_user;
	}
}

void del_user(const struct userlist * user)
{
	struct userlist *cur, *prev;

	if(head == NULL || user == NULL)
		return;

	cur  = head;
	prev = NULL;
	while(cur) {
		if(strcmp(cur->name, user->name) == 0 &&
		   strcmp(cur->host, user->host) == 0 &&
		   cur->s_addr == user->s_addr)
		{
			break;
		}
		else{
			cur  = cur->next;
			prev = cur;
		}
	}

	if(cur) {
		prev->next = cur->next;	
		free(cur);
	}

}



int get_addr_by_name(const char *name)
{
	struct userlist *p=head;
	while(p)
	{
		if(strcmp(p->name, name)==0)
		{
			return p->s_addr;
		}
		p = p->next;
	}	
	return 0;
}

void show_users()
{
	struct userlist *p=head;
	printf("%10s\t%10s\t%s\n", "Username","Hostname","IP Address");
	while(p)
	{
		struct in_addr addr = {p->s_addr};
		printf("%10s\t%10s\t%s\n",p->name,p->host,inet_ntoa(addr));
		p=p->next;
	}	
}

void free_userlist()
{
	struct userlist *cur, *prev;	

	if(head == NULL)
		return ;

	cur  = head;
	prev = NULL;
	while(cur)
	{
		prev = cur;
		cur  = cur->next;
		free(prev);
	}
}
