#ifndef THREADPOOL_H

#define THREADPOOL_H

#include "base.h"
#include "connpool.h"

/*
 *传入线程函数的参数结构体
 * */
typedef struct {
	int epfd;//epoll管理的文件描述符
	int connpool_no;//连接池编号
}workerarg;

/*
 *线程池结构
 * */
typedef struct pthread_pool{
	//todo 两个变量并没有使用  因为这个http服务器可以不用  线程一旦启动便是循环处理
	pthread_mutex_t threadlock;/*线程池互斥量*/
	pthread_cond_t taskcond;//task条件变量
	workerarg *args;/*线程池参数信息集*/
	pthread_t *pthreads;/*线程池ID集*/

	int threadnum;/*线程池线程数量*/
	int cur_workid;/*应该分发任务的线程id（自定义的0-threadnum - 1）*/
}thread_pool;

/*
 * 工作线程start function
 * */
void* thread_work(void* arg);
/*
 *初始化线程池
 * */
bool init_threadpool(int threadnum);

/**
 * 销毁线程池
 * */
bool destroy_threadpool();
/*
 * 向任务队列添加任务：
 * 参数：workerid:指定的工作线程， sockfd:要处理的sockfd
 * */
bool add_task(int connfd);
/*
 * 处理EPOLLIN事件
 * */
bool deal_epoll_in(struct threadarg * arg, int epfd, epoll_event evnt);
/*
 *处理EPOLLOUT事件
 * */
bool deal_epoll_out(struct threadarg * arg, int epfd, epoll_event evnt);

#endif
