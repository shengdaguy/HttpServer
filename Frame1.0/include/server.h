#ifndef SERVER_H
#define SERVER_H

#include "base.h"
#include "log.h"
#include "connpool.h"
#include "threadpool.h"
#include "config.h"
//自定义客户端socket结构
struct connsocket{
        int connfd;
        struct sockaddr_in conn_addr;
};
//自定义服务端socket结构
struct serversocket {
	int sockfd;
	struct sockaddr_in server_addr;
};


void ginit();
bool init_socketbuf(int sockfd);
void start_listen(struct serversocket* sv);
void gdestroy();
void* listen_task(void*);
bool set_nonblocking(int sock);		//设置为非阻塞套接字
struct serversocket * init_serversocket(struct serversocket *);
#endif
