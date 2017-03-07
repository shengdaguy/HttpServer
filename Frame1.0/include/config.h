#ifndef CONFIG_H
#define CONFIG_H

#include "base.h"

bool init_config();
int get_threadpoolsize();
int get_connbufsize();
char* get_ip();
int get_serverport();
int get_conntimeout();
int get_epolltimeout();
int get_logfilesize();
int get_connpoolsize();
int get_maxevents();
int get_backlog(); 
int get_so_recvbuf(); 
int get_so_sendbuf(); 

#endif 
