#include "../include/base.h"
#include "../include/util.h"
#include "../include/log.h"
#include "../include/config.h"
#include "../include/err.h"

//使用绝对路径指定log的存储地址  使用chdir切换
static char LOGPATH[100] = "/home/lihonglin/httpserverV1/Frame1.0/logfile/";
static char* filename = NULL;
static int cur_bytes;
static pthread_mutex_t loglock;

/*
 *生成新的logfile文件名：
 格式：logfileyyyymmddhhmmss
 不知道为什么hour总是0
 */
char* newfilename() {
	char *name = new char[100];
	time_t tt = time(NULL);
	struct tm* cur_time = gmtime(&tt);
	sprintf(name, "%s_%d_%d_%d_%d_%d_%d", "log", cur_time->tm_year + 1900, cur_time->tm_mon + 1, cur_time->tm_mday, cur_time->tm_hour + 8, cur_time->tm_min, cur_time->tm_sec);
	return name;
}
//日志功能启动操作
void init_log() {
	if (filename == NULL) {
		filename = newfilename();
		if (filename == NULL) {
			err_sys("log newfilename error", 1);
		}
	}
	cur_bytes = 0;
	int err	= pthread_mutex_init(&loglock, NULL);/*初始化日志锁*/
	if (err) {
		err_sys(strerror(err), 1);
	}
}
//终结程序是调用 相当于析构函数 
void destroy_log() {
	delete filename;
	pthread_mutex_destroy(&loglock);
}
//加锁 使日志功能是线程安全的
void writelog(const char* logcontent) {

	pthread_mutex_lock(&loglock);
	real_write(logcontent);
	pthread_mutex_unlock(&loglock);

}

/*
 * 将logcontent打印到日志文件中。
 * 加上换行 并记录当前日志文件的字节数。
 * */
void real_write(const char* logcontent) {
	//printf("%s\n", logcontent);
	FILE *fp;
	//如果是程序运行第一次写日志 或者日志文件大小已经大于最大 则回滚
	if (cur_bytes > MAXLOGSIZE)	{
		delete[]filename;
		filename = newfilename();
		cur_bytes = 0;
	}
	char tempfile[100] = "./";
	strcat(tempfile, filename);
	chdir(LOGPATH);
//	print_cwd();
//	system("pwd");
//	打开日志文件
	fp = fopen(tempfile , "a+");
	//puts(tempfile);
	if (fp != NULL) {
//		fputs(logcontent, fp);
	//	printf("%s\n", logcontent);
		fprintf(fp, "%s\n", logcontent);
		cur_bytes += strlen(logcontent) + 1;
		if (cur_bytes > get_logfilesize()) {
			delete []filename;
			filename = newfilename();
		}
		if (fclose(fp)) {
			err_sys("关闭日志文件出错", 0);
		}
	}
	else {
		err_sys("写入日志出错", 0);
	}
}
