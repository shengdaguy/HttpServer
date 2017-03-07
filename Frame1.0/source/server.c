#include "../include/server.h"
#include "../include/err.h"

#define DEBUG

int main(int, char**)
{
#ifdef DEBUG

freopen("/home/lihonglin/httpserverV1/Frame1.0/tempout", "w", stdout);

#endif
	struct serversocket* sv, sv_entity;
	ginit();//初始化服务器
	//启动监听线程
	if ((sv = init_serversocket(&sv_entity)) != NULL) {
		start_listen(sv);
	}
	gdestroy();//服务器关闭(析构)函数
	return 0;

}
/*开启监听线程*/
void start_listen(struct serversocket* sv) {
	//todo err handler	
	pthread_t lispid;
    pthread_create(&lispid, NULL, listen_task, sv);
	pthread_join(lispid, NULL);/*等待监听线程返回*/

}
/*
 *服务器启动后的准备
 */
void ginit() {

	/*初始化日志功能*/
	init_log();//todo err handler
	/*初始化配置文件*/
	if (!init_config()) {
		err_sys("初始化配置出错", 1);
	}
	if (!init_threadpool(get_threadpoolsize())) {
		err_sys("初始化线程池失败", 1);
	}//初始化线程池
}


bool init_socketbuf(int sockfd) {
	int so_sendbuf, so_recvbuf;
	
	so_sendbuf = get_so_sendbuf();
	so_recvbuf = get_so_recvbuf();
	setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &so_sendbuf, sizeof(so_sendbuf));
	setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &so_recvbuf, sizeof(so_recvbuf));

	return true;
}

/*
 * 监听线程对连接任务的管理
 */
void* listen_task(void* arg) {
	int cnt = 0;/*连接数目统计*/
	char templog[100];
	struct serversocket* sv = (serversocket*)arg;
	//初始化epoll管理
	struct connsocket cs;
	struct epoll_event * events, ev;
	int nfds, epfd;

	events = new epoll_event[get_maxevents()];
	if (!events) {
		err_sys("new listen thread events error", 1);
	}
	epfd = epoll_create(get_maxevents());//创建用于epoll管理的epoll文件描述符
	ev.data.fd = sv->sockfd;
	ev.events = EPOLLIN | EPOLLET;
	//注册服务器端的事件监听： 可写 + 边缘触发
	epoll_ctl(epfd, EPOLL_CTL_ADD, sv->sockfd, &ev);

	while (1) {
//		printf("监听线程开始工作\n");
		nfds = epoll_wait(epfd, events,get_maxevents() , get_epolltimeout());
//		printf("nfds = %d\n", nfds);
		for (;nfds;) {
			/*
			 * nfds总是1.但实际上最好多次accept把连接取出。特别是ab压测
			 * 并发量比较大的情况
			 */
			uint32_t clilen = sizeof(struct sockaddr_in);
			cs.connfd = accept(sv->sockfd, (SA *)&(cs.conn_addr), &clilen);
			if (cs.connfd < 0) {
				break;//todo err handler
			}
			sprintf(templog, "增加来自客户端的连接cnt = %d  sockfd为 %d",cnt++, cs.connfd);
			writelog(templog);//记入日志
			//成功建立三路握手连接后 添加任务到任务集合taskset
			if (!set_nonblocking(cs.connfd)) {
				perror("set nonblocking error");
			}
			init_socketbuf(cs.connfd);
			add_task(cs.connfd);
		}
	}
	delete []events;//释放内存
	close(epfd);//关闭epoll文件
	pthread_exit((void*)0);
}

/* 
 *服务器关闭前的清理
 * */
void gdestroy() {
	destroy_log();//关闭日志
	destroy_threadpool();//销毁线程池
}

bool set_nonblocking(int sock)		//设置为非阻塞套接字
{
	/*先获取后添加*/
	int flags = fcntl(sock, F_GETFL, 0);
	if( flags == -1)	
		return false;
	flags |= O_NONBLOCK;
	int ret = fcntl(sock, F_SETFL,  flags);
	
	return ret != -1;
}
/*
 *初始化服务端socket :socket bind listen 
 如果成功返回指向struct serversocket的指针。
 错误返回空指针。
 * */
struct serversocket* init_serversocket(struct serversocket* sv) {
	if ((sv->sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		err_sys("socket 创建出错", 1);
	}
	sv->server_addr.sin_family = AF_INET;
//	使用配置文件里的端口设置
	sv->server_addr.sin_port = htons(get_serverport());
	//sv->server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
//	sv->server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	inet_pton(AF_INET, get_ip(),&(sv->server_addr.sin_addr.s_addr));
	int on = 1;//设置连接重用
	if(setsockopt(sv->sockfd, SOL_SOCKET, SO_REUSEADDR, &on, (socklen_t)sizeof(on)) < 0)
	{
		perror("set error");
	}
	if (!set_nonblocking(sv->sockfd)) {//设置非阻塞套接字
		perror("set nonblocking error");
	}
	bzero(&(sv->server_addr.sin_zero), 8);
    if (bind(sv->sockfd, (SA *) &sv->server_addr, sizeof(struct sockaddr)) == -1) {
		err_sys("bind 出错", 0);
		return NULL;
	  }
	
	  if (listen(sv->sockfd, get_backlog()) == -1) {
		err_sys("listen出错", 0);
		return NULL;
	  }
	return sv;
}
