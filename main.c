/* ************************************************************************
 * Filename: ipmsg.c
 * Description: main.c
 * Version: 1.0
 * Created: 2010年03月08日 10时28分50秒
 * Revision: none
 * Compiler: gcc
 * Author: YOUR NAME (),
 * Company:
 * Maintainer: wenhao v1.1 2011-11-10
	1. 修正在输入状态时,输入空格出段错误的BUG
	2. 修正收到SENDCHECKOPT时回复包编号默认为0的BUG
	3. 修正help打印内容规范问题
 * Maintainer: hongjin.cao 2014-5-19
	TODO:
		* get user host from command line (argv) *2014-5-20 DONE*
		* use snprintf instead of strcpy, strncpy   *2014-5-19 DONE*
		* add necessary const to function arguments
		* use MAX_BUFF instead of a number *DONE*
		* why all the thread use same tid???
		* add log
		* add history of communication
		* add gui
		* tab complete user name
	BUGS:
		* 如果用户用户名相同，信息会发送错误
			应该检查用户名是否存在，如果存在则提示用户，然后给用户名提供一个
			没有用到的用户名，或用户换一个没有用到的用户名
		* 发送大文件的时候，进度现实不正常。
 * ************************************************************************/
#include "myinclude.h"
#include "time.h"
#include "communication.h"
#include "user_manager.h"
#include "user_interface.h"
#include "lib.h"

#define DEFAULT_USER "ipmsg_defuser"
#define DEFAULT_HOST "ipmsg_defhost"

int main(int argc, char *argv[])
{
	pthread_t tid;	
	const char *user;
	const char *host;
	int c;

	// get user,host from command line
	user = DEFAULT_USER; // default value
	host = DEFAULT_HOST;
 	while ( (c = getopt(argc, argv, "u:h:")) != -1) {
		switch (c) {
		case 'u':
			user = optarg;		
			break;

		case 'h':
			host = optarg;	
			break;
		}
	}
	if (optind == 1)
		handle_warning("you don't provide user and host. so use the default \
value\nusage: ipmsg  [ -u user ] [ -h host ]\n");
	printf("user:%s\nhost:%s\n", user, host);

	// 初始化系统
	online(user, host);

	// 接收消息线程，接收其他客户端发送的UDP数据
	pthread_create(&tid, NULL, recv_msg_thread, NULL);

	// 用户界面线程，处理用户输入的命令
	pthread_create(&tid, NULL, user_interface, NULL);

	// 发送文件线程，等待其他客户端接收文件并负责向其传送文件
	pthread_create(&tid, NULL, sendfile_thread, NULL);

	// 主线程不能退出
	pthread_join(tid, NULL);

	return 0;	
}
