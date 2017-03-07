#include "../include/base.h"
#include "../include/err.h"


/*
 从文件描述符fd里读取n字节。存储到vptr中。
 知道出错 读到文件结束符 读满n字节
 */
ssize_t Readn(int fd, void * vptr, size_t n) {
	size_t nleft;
	ssize_t nread;
	char *ptr;
	ptr = (char*)vptr;
	nleft = n;

	while(nleft > 0) {
		if ((nread = read(fd, ptr, nleft)) < 0)	 {
			if (errno == EINTR) {
				nread = 0;
			}
			else return -1;
		}
		else if (nread == 0) break;
		nleft -= nread;
		ptr += nread;
	}

	return (n - nleft);
}
/*
 * 像文件fd里写入n字节的内容
 */
ssize_t Writen(int fd, void* vptr, size_t n) {

	size_t nleft;
	ssize_t nwritten;
	const char *ptr;
	
	ptr = (char*)vptr;
	nleft = n;
	while (nleft > 0)	 {
		if ((nwritten = write(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR) {
					nwritten = 0; /*and call write() again */
			}
			else 
				return -1;
		}
		nleft -= nwritten;
		ptr += nwritten;
	}

	return n;
}
/*
 *从文件fd里读取一行。
 */
ssize_t readline(int fd, void *vptr, size_t maxlen) {

	ssize_t n, rc;
	char c, *ptr;
	for (n = 1; n < maxlen; n++) {
		again:
			if ((rc = read(fd, &c, 1)) == 1) {
				*ptr++ = c;
				if (c == '\n') break;
			}
			else if (rc == 0) {
				*ptr = 0;
				return n - 1;
			}
			else {
				if (errno == EINTR) 
					goto again;
				return -1;
			}
	}
	*ptr = 0;
	return n;
}


