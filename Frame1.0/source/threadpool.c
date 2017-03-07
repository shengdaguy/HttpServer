#include "../include/threadpool.h"
#include "../include/err.h"
#include "../include/connpool.h"
#include "../include/config.h"
#include "../include/base.h"
#include "../include/io.h"

#define MAXDATASIZE 1024
//#define DEBUG
thread_pool *threadpool;

static char SERVICESO[100] = "../http_lib/http_service.so";
typedef int (*check_com) (char* buf, int len);
typedef char* (*handler_req) (conn* buf);

void* dll;
check_com check_complete;
handler_req handler_request;
/*
 *动态引用共享库
 */
void* init_dl() {
	void* pdlhandler = dlopen(SERVICESO, RTLD_LAZY);
	char *pszerr = dlerror();
	if (!pdlhandler || pszerr) {
		perror("加载动态链接库出错");
		return NULL;
	}
	check_complete = (check_com)dlsym(pdlhandler, "check_complete");
	handler_request = (handler_req)dlsym(pdlhandler, "handler_request");
	if (check_complete && handler_request)
		return pdlhandler;
	return NULL;
}

bool deal_epoll_in(int epfd, int connpool_no, epoll_event ev) {
	int connfd;//客户端socket描述符
	connfd = ev.data.fd;
	//从连接池重获得存有读写缓冲区的数据结构data
	conn *tempconn = getconn(connpool_no, connfd);
	int len;
	//把读取的字节append到读写缓冲区。
	//因为是边缘触发 + 非阻塞套接字：要小心检查read 的返回值小于0且errno为EAGAIN 的情况
	//TODO 下行代码优化 a.代码丑  b.如果请求头过长可能不能很好的处理
	if ((len = recv(connfd, tempconn->recvbuf + tempconn->request_len, MAXDATASIZE - tempconn->request_len, 0)) < 0) {
		perror("read error");//todo 直接断开连接不友好
		delete_conn(connpool_no, connfd);
		epoll_ctl(epfd, EPOLL_CTL_DEL, connfd, &ev);
		return false;
	}
	//这种情况往往是客户端断开连接 (epoll返回说一个套接字可度 结果什么都没读出来)
	else if (len == 0) {
		printf("客户端主动断开连接\n");
		delete_conn(connpool_no, connfd);
		epoll_ctl(epfd, EPOLL_CTL_DEL, connfd, &ev);
		return false;//已经删除连接池的连接 怎么可以继续更新呢？ 所以一直core dump...
	}
#ifdef DEBUG
	puts(tempconn->recvbuf);
#endif 
//	TODO 检查包的完整性也许更应该放在这里。不完整继续等待 完整改为监听out事件  错误直接关闭连接 并返回输出
	//puts(tempconn->recvbuf);
	tempconn->request_len += len;	
	//包不完整继续等待 包完整更改监听事件 包数据错误等到 epoll_out事件处理
	if (((*check_complete)(tempconn->recvbuf, tempconn->request_len))) {
		ev.events = EPOLLOUT | EPOLLET;
		epoll_ctl(epfd,EPOLL_CTL_MOD,connfd,&ev);
		return true;
	}
	puts("the package is not complete");
}

bool deal_epoll_out(int epfd, int connpool_no, epoll_event ev) {
	int connfd;//客户端socket描述符
	connfd = ev.data.fd;
	//从连接池获取data*
	conn *tempconn = getconn(connpool_no, connfd);
	//TODO service 的调用逻辑有待梳理
	if((handler_request(tempconn))) {//处理请求连接之后 保持keepalive
		update_conn(connpool_no, connfd, 0);
		ev.events = EPOLLIN|EPOLLET;
		epoll_ctl(epfd,EPOLL_CTL_MOD,connfd,&ev);
		#ifdef DEBUG
		puts("epoll out right");
		#endif
		return true;
	}else {//需要断开连接 (友好的提示信息已经发送到客户端)
		puts("epoll out wrong");
	//	sleep(1);//等待一秒  以保证客户端尽量收到错误的提示信息 (todo 待改进)
		delete_conn(connpool_no, connfd);
		epoll_ctl(epfd, EPOLL_CTL_DEL, connfd, &ev);
		return false;
	}
	//	delete_conn(connfd);//连接池删除功能去了 所以用wget 和 curl请求是无法断开的。。。
}


void* thread_work(void* arg) {
	int err;
	workerarg *temparg = (workerarg*)arg;//转换接受参数
	int nfds, epfd, connpool_no;
	struct epoll_event *events, ev;//epoll事件管理


	epfd = temparg->epfd;
	connpool_no = temparg->connpool_no;
	events = new epoll_event[get_maxevents()];//todo err
	//TODO service

	while(1) { /* 工作线程开始工作 epoll管理socket连接 本来的结构可能造成死锁，现在选择死循环处理task*/
			nfds = epoll_wait(epfd, events, get_maxevents(), get_epolltimeout());
			for (int i = 0; i < nfds; i++) {
			 if (events[i].events & EPOLLIN) {//请求是读
					 deal_epoll_in(epfd, connpool_no, events[i]);
				}
				else if (events[i].events & EPOLLOUT) {//socket写事件
		    	    deal_epoll_out(epfd, connpool_no,events[i]);
				}
				 else if (events[i].events & EPOLLERR) {
					//TODOl err
				}
		}//end of for (i = 0; i < nfds; i++);
		//TODO timeoutconn
		delete_conntimeout(connpool_no);
	}
}//end of fun();


/*产生新的工作线程*/
int new_worker(pthread_t *pt, workerarg *args, int i) {
	args[i].epfd = epoll_create(get_maxevents());
	args[i].connpool_no = get_connpool_no();//todo err handler
	workerarg * temp = args + i;//因为多线程 执行顺序不缺定 所以要用中间变量传递
	int err = pthread_create(pt + i, NULL,thread_work ,(void*)temp);
	return err;
}
/*
 *初始化线程池
 * */
bool init_threadpool(int threadnum) {
	int err;
	threadpool = new thread_pool;
	
	threadpool->threadnum = threadnum;//记录连接池连接数目
	threadpool->cur_workid = 0;//初始化分配线程id
	threadpool->pthreads = new pthread_t[threadnum];
	threadpool->args = new workerarg[threadnum];
	if (threadpool->pthreads == NULL) {
		delete []threadpool->pthreads;
		return false;
	}
	for (int i = 0; i < threadnum; i++) {//这里可以定义一个包裹函数性质的函数封装过程
		err = new_worker(threadpool->pthreads, threadpool->args, i);/*创建新的工作线程*/
		if (err < 0) {
			delete []threadpool->pthreads;
			return false;
		}
	}
	//在初始化连接池里加载  因为如果让工作线程加载感觉浪费  如果是多进程可能更适合每个worker分别加载
	if ((dll = init_dl()) == NULL) {
		err_sys("加载动态逻辑出错", 1);
	}
	return true;
}

/**
 * 销毁线程池
 * */
bool destroy_threadpool() {

	for (int i = 0; i < threadpool->threadnum; i++) {
		pthread_join(threadpool->pthreads[i], NULL);//等待线程退出，防止僵尸进程
	}
	delete []threadpool->pthreads;//释放连接id内存 参数内存 连接池自身内存
	delete[] threadpool->args;
	delete threadpool;
	if (dll) dlclose(dll);
	return true;
}
/*
 * 向任务队列添加任务：
 * Round_robin算法
 * 参数：sockfd:要处理的sockfd
 */
bool add_task(int connfd) {
	int err;
	workerarg *temp = &(threadpool->args[threadpool->cur_workid]);//获取要分配的线程的参数(round-robin算法) 这个要用指针
	epoll_event ev;
	add_conn(temp->connpool_no, connfd);//向连接池添加连接
	ev.data.fd = connfd;
	ev.events = EPOLLIN | EPOLLET;//初始化epoll添加事件 epoll_event
	epoll_ctl(temp->epfd, EPOLL_CTL_ADD, connfd, &ev);//向线程epoll管理添加事件
	threadpool->cur_workid = (threadpool->cur_workid + 1) % (threadpool->threadnum);//维护连接池的下一个分配线程id
#ifdef DEBUG
	printf("add task connfd %d success\n", connfd);
#endif
	if (err != 0) {
		printf("error: %s\n", strerror(err));
		return false;
	}
}
