#include "../include/base.h"
#include "../include/err.h"
#include "../include/config.h"
#include "../include/connpool.h"

#define MAX_CONNPOOL 200

map<int, conn*> connpoolmaps[MAX_CONNPOOL];
static int connpool_cnt;
/*
 * 初始化连接池
 * 有可能需要在这里添加代码：目前没有
 * 采用每个工作进程一个连接池调用 传递map 
 * 不知道map的实现机制传递map是传递一个地址？ 如果是复制内容那么太奢侈了 需要改进
 * 也许可以试试引用&
 * */
bool init_connpool() {
	//todo err
	connpool_cnt = 0;
	return true;
}
/*
 *返回连接池编号
 */
int get_connpool_no() {
	if (connpool_cnt < MAX_CONNPOOL) {
		return connpool_cnt++;
	}
	return -1;
}
/*
 *向连接池里添加连接
 key 是传入的index（sockfd）
 初始化缓冲区 访问时间等
 * */
bool add_conn(int coonpool_no, int key) {
//	printf("向连接池里添加连接 其sockfd = %d\n", index);
	conn  *tempconn = new conn;
	tempconn->connfd = key;
	tempconn->request_len = 0;
	tempconn->last_time = time(NULL);
	connpoolmaps[coonpool_no].insert(make_pair(key, tempconn));
	return true;
}
/*
 *清空连接池
 * */
bool clear_connpool(int connpool_no) {
	printf("清空连接池\n");
	map<int, conn*> :: iterator iter;
	for (iter = connpoolmaps[connpool_no].begin(); iter != connpoolmaps[connpool_no].end(); iter++) {
		delete_conn(connpool_no, iter->first);
	}
	return true;
}
/*
 *清除超时连接
 *
 */
bool delete_conntimeout(int connpool_no) {
	map<int, conn*> :: iterator iter;

	for (iter = connpoolmaps[connpool_no].begin(); iter != connpoolmaps[connpool_no].end(); iter++) {
		//便利map 获得当前时间和上一次时间的差值
		int diff = difftime(time(NULL), iter->second->last_time);
		//printf("connfd = %d overtime = %d\n",iter->first, diff);
		//如果大于配置文件里设置的值则从连接池里删除连接
		if (diff >= get_conntimeout()) {
			delete_conn(connpool_no, iter->first);
		}
	}
	return true;
}
/*更新连接池连接*/
bool update_conn(int connpool_no, int key, int len) {
	if (len == 0) {
		connpoolmaps[connpool_no][key]->request_len = 0;
	}
	return true;
}
/*
 *删除连接池里的连接：
 1.delete new 的data
 2.close sockfd
 3.删除map的键值对
 */
bool delete_conn(int connpool_no, int key) {
	delete connpoolmaps[connpool_no][key];
	connpoolmaps[connpool_no].erase(key);
	close(key);
	return true;
}

/*
 * 根据index获得连接池里的data指针
 */
conn* getconn(int connpool_no, int key) {
	return connpoolmaps[connpool_no][key];
}
