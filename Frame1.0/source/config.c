#include "../include/base.h"
#include "../include/err.h"
#include "../include/util.h"

static int serverport;
static int conntimeout;
static int logfilesize; 
static int connbufsize;
static int connpoolsize;
static int backlog;
static int epolltimeout;
static int threadpoolsize;
static int maxevents;
static int so_sendbuf;
static int so_recvbuf;

static char ip[30];
/*
 * 读取配置文件并初始化。
 * 对外提供接口
 * 其中参数的定义在.cpp源文件里定义
 * 成功返回true ,失败返回false.
 * */

bool init_config() {
	char line[200], key[100];
	char filename[100] = "../server.conf"; /*设置配置文件文件名*/
	FILE * fp;
	int value;
	
	//print_cwd();
	if ((fp = fopen(filename, "r")) == NULL) {
		err_sys("打开配置文件出错", 1);
	}
	//按行读取。读取后的功能扩展代码可以放在这里
	while (fgets(line, sizeof(line),fp) != NULL) {
		sscanf(line, "%s", key);
		//todo err
		//todo 更好的实现方式
		if (key[0] == '#') continue;//略过注释
		if (strcmp(key, "serverport") == 0) {
			sscanf(line, "%*s = %d", &value);
			serverport = value;
		//	printf("%s %d\n",key, value);
		}
		else if (strcmp(key, "connbufsize") == 0) {
			sscanf(line, "%*s = %d", &value);
			connbufsize = value;
			//printf("%s %d\n",key, value);
		}else if (strcmp(key, "epolltimeout") == 0) {
			sscanf(line, "%*s = %d", &value);
			epolltimeout = value;
	//		printf("%s %d\n",key, value);
		}else if (strcmp(key, "threadpoolsize") == 0) {
			sscanf(line, "%*s = %d", &value);
			threadpoolsize = value;
		//	printf("%s %d\n",key, value);
		}
		else if (strcmp(key, "logfilesize") == 0) {
			sscanf(line, "%*s = %d", &value);
			logfilesize = value;
	//		printf("%s %d\n",key, value);
		}
		else if (strcmp(key, "maxevents") == 0) {
			sscanf(line, "%*s = %d", &value);
			maxevents = value;
		//	printf("%s %d\n",key, value);
		}
		else if (strcmp(key, "backlog") == 0) {
			sscanf(line, "%*s = %d", &value);
			backlog = value;
	//		printf("%s %d\n",key, value);
		}
		else if (strcmp(key, "connpoolsize") == 0) {
			sscanf(line, "%*s = %d", &value);
			connpoolsize = value;
		//	printf("%s %d\n",key, value);
		}else if (strcmp(key, "conntimeout") == 0) {
			sscanf(line, "%*s = %d", &value);
			conntimeout = value;
	//		printf("%s %d\n",key, value);
		}else if (strcmp(key, "so_sendbuf") == 0) {
			sscanf(line, "%*s = %d", &value);
			so_sendbuf = value;
	//		printf("%s %d\n",key, value);
		}else if (strcmp(key, "so_recvbuf") == 0) {
			sscanf(line, "%*s = %d", &value);
			so_recvbuf = value;
	//		printf("%s %d\n",key, value);
		}else if (strcmp(key, "ip") == 0) {
			sscanf(line, "%*s = %s", ip);
		}
	}
	//关闭文件
	fclose(fp);//todo err
	return true;
}


int get_threadpoolsize() {
	return threadpoolsize;
}
int get_connbufsize() {
	return connbufsize;
}
int get_serverport() {
	return serverport;
}
int get_conntimeout() {
	return conntimeout;
}

int get_epolltimeout() {
	return epolltimeout;
}

int get_logfilesize() {
	return logfilesize;
}
int get_connpoolsize() {
	return connpoolsize;
}
int get_maxevents() {
	return maxevents;
}
int get_so_sendbuf() {
	return so_sendbuf;
}
int get_so_recvbuf() {
	return so_recvbuf;
}
int get_backlog() {
	return backlog;
}
char* get_ip() {
	return ip;
}
