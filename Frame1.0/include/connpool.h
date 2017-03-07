#ifndef CONNPOOL_H

#define CONNPOOL_H

#define BUFSIZE 8192
//连接池的连接的数据结构
//todo 更完善的功能和设计
typedef struct connection{
	int connfd;//连接描述符
	//自定义缓冲区
	char recvbuf[BUFSIZE];
	char sendbuf[BUFSIZE];
	ssize_t request_len;
	//上次访问时间
	time_t last_time;
}conn;

bool init_connpool();
int get_connpool_no();
bool update_conn(int connpool_no, int key, int len);
bool add_conn(int connpool_no, int key);
bool clear_connpool(int connpool_no);
bool delete_conn(int connpool_no, int key);
bool delete_conntimeout(int connpool_no);
conn* getconn(int connpool_no, int key);

#endif

