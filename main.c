/* ************************************************************************
 *       Filename:  ipmsg.c
 *    Description:  main.c
 *        Version:  1.0
 *        Created:  2010年03月08日 10时28分50秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 
 
 *     Maintainer: wenhao  v1.1 2011-11-10
				   1. 修正在输入状态时,输入空格出段错误的BUG
				   2. 修正收到SENDCHECKOPT时回复包编号默认为0的BUG
				   3. 修正help打印内容规范问题
 * ************************************************************************/
#include "myinclude.h"
#include "time.h"
#include "communication.h"
#include "user_manager.h"
#include "user_interface.h"

int main(int argc, char *argv[])
{
	pthread_t tid;	
	online("Sunplusapp", "root_teacher");
	
	//接收消息线程，接收其他客户端发送的UDP数据
	pthread_create(&tid, NULL, recv_msg_thread, NULL);
	
	//用户界面线程，处理用户输入的命令
	pthread_create(&tid, NULL, user_interface, NULL);
	
	//发送文件线程，等待其他客户端接收文件并负责向其传送文件
	pthread_create(&tid, NULL, sendfile_thread, NULL);
	//主线程不能退出
	
	pthread_join(tid, NULL);
	return 0;	
}
