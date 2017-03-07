#include "../include/base.h"


/*
 * 处理程序运行时错误。
 * 参数： 打印到标准错误的字符串   是否退出及退出返回值
 */

void err_sys(const char* fmt, int opt) {

	perror(fmt);	
	if (opt) exit(opt);//如果fatal错误 退出程序

}
