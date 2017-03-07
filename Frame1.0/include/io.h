#ifndef IO_H
#define IO_H

#include "base.h"
ssize_t Readn(int filedes, void * buff, size_t nbytes);
ssize_t Writen(int fieldes, void* buff, size_t  nbytes);
ssize_t Readline(int filedes, void* buff, size_t nbytes);

#endif

