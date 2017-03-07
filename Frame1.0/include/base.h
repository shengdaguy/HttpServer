/*
 * 这里是基础的头文件和一些宏定义以及类型定义
 * */
#ifndef BASE_H

#define BASE_H
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <cmath>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <dlfcn.h>
#include <netinet/tcp.h>


using namespace std;

#define SA struct sockaddr


#endif
