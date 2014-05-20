/*

File name: lib.h 
Description: 存放一些常用函数,宏定义
Author: huntinux@gmail.com
Date: 2014-5-20
 
 */

#ifndef _LIB_H
#define _LIB_H

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define handle_warning(msg) \
	do { fprintf(stderr, "%s", msg); } while(0)

#endif // _LIB_H
