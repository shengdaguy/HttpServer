#ifndef LOGFILE_H

#define LOGFILE_H
#define MAXLOGSIZE (20*1024*1024)

void init_log();//初始化log功能
void destroy_log();//销毁log功能
char* newfilename();//新的函数名
void writelog(const char*);//写日志 加锁处理文件读写
void real_write(const char*);//真正的写入文件
#endif
