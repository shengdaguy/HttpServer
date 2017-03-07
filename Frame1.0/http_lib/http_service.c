#include "../include/base.h"
#include "../include/connpool.h" 
#include "http_service.h"

#define MAXBUFSIZE 409600
#define SEND_CHUNK_SIZE 4096
static char CHECK_COMPLETE_STRING[20] = "\r\n\r\n";
static char HTTP_VERSION[100] = "HTTP/1.1";
static char DOC_ROOT[100] = "/home/lihonglin/httpserverV1/Frame1.0/docroot"; 


//debug
void debug_print_request(http_request_t *request) {
	printf("request clientfd: %d\n", request->clientfd);
	printf("request version: %s\n", request->startline->version);
	printf("request file: %s\n", request->startline->filename);
	printf("request method: %d\n", request->startline->method);
}

void debug_print_response(http_response_t *response) {
	printf("response clientfd: %d\n", response->clientfd);
	printf("response status code: %d\n", response->startline->status_code);
	printf("response version: %s\n", response->startline->version);
	printf("response reason phrase: %s\n", reason_entity[response->startline->reason_phase]);
//	printf("response body: %s\n", response->body->content);
	printf("response content length: %d\n", response->body->content_length);
}

#ifdef DEBUG
#endif

/*
 *处理客户端发送数据未服务端发送数据包
 *
 */
extern "C" bool handler_request(conn* connetion) {
//	puts(connetion->recvbuf);
	http_request_t request;
	http_response_t response;
#ifdef DEBUG
	printf("request address %p\n", request);
#endif
	//init request & response
	init_http_request(&request);
	init_http_response(&response);
	//transfer connetion to request
	conn_to_request(connetion, &request);
//	debug_print_request(&request);
	parse_request(&request, &response);
//	debug_print_response(&response);
	send_response(&response);
	puts("connection handler finish");	
	return true;
}


//TODO
bool conn_to_request(conn* connection, http_request_t *request ) {
	request->clientfd = connection->connfd;
	get_request_startline(request->startline,connection->recvbuf);
	get_request_header(request->header,connection->recvbuf);
	get_request_body(request->body, connection->recvbuf);
	
	return true;
}

//TODO
bool parse_request(http_request_t *request, http_response_t *response){
	
	response->clientfd = request->clientfd;

	switch(request->startline->method) {
		case GET: handler_get(request, response);
					break;
		case POST: handler_post(request, response);
					break;
		case PUT: handler_put(request, response);
					break;
		case DELETE: handler_delete(request, response);
					break;
		case OPTIONS: handler_options(request, response);
					break;
		case HEAD: handler_head(request, response);
					break;
		case TRACE: handler_trace(request, response);
					break;
		case NOT_KNOW: handler_method_not_allowed(request, response);
					break;
	}
}





//handler by method
void handler_get(http_request_t *request, http_response_t *response) {
	char filename[200] = "./";
	struct stat statfile;
	FILE *fp;

	strcat(filename, request->startline->filename);
	chdir(DOC_ROOT);
	if ((fp = fopen(filename, "r")) == NULL) {
		handler_not_found(request, response);
	}else {
		int err = stat(filename, &statfile);
		//TODO 访问docroot之外的文件
		//	   内容过大的文件
		//	   等
		if (statfile.st_size >= MAXBUFSIZE) {
			handler_forbidden(request, response);
			return;
		}
	
		//判断是否超出docroot
		char path[300];
		strcpy(path, DOC_ROOT);
		char *end = strrchr(filename, '//');	
		if (end) *end = 0;
		strcat(path, filename + 2);
		chdir(path);
//		puts(path);
		char temppath[300];
		int len = 300;
		if (getcwd(temppath, len) != NULL) {
	//		puts(temppath);
	//		puts(DOC_ROOT);
			if (strstr(temppath, DOC_ROOT) == NULL) {
				handler_forbidden(request, response);
			}
		}
		//读取文件内容 我只能说这样不好
		int c;
		int i = 0;//这个最好用read write fgets会在最后添加\n
		while ((c = fgetc(fp)) != EOF) {
			response->body->content[i++] = (char)c;
		}
		response->body->content_length = i;
		fclose(fp);
		handler_ok(request, response);
		
	}
}

void handler_post(http_request_t *request, http_response_t *response){
	handler_method_not_allowed(request, response);
}
void handler_put(http_request_t *request, http_response_t *response){
	handler_method_not_allowed(request, response);
}
void handler_delete(http_request_t *request, http_response_t *response){
	handler_method_not_allowed(request, response);
}
void handler_head(http_request_t *request, http_response_t *response){
	handler_method_not_allowed(request, response);
}
void handler_trace(http_request_t *request, http_response_t *response){
	handler_method_not_allowed(request, response);
}
void handler_options(http_request_t *request, http_response_t *response){
	handler_method_not_allowed(request, response);

}

//handler by result
void handler_ok(http_request_t *request, http_response_t *response) {

	response->startline->status_code = HTTP_OK;
	strcpy(response->startline->version , HTTP_VERSION);
	response->startline->reason_phase = OK;

	//TODO file body
	http_table_elt_t tempheader1, tempheader2, tempheader3;
	strcpy(tempheader1.key , "Connection");
	strcpy(tempheader1.value , "keep-alive");
	strcpy(tempheader2.key, "Content-Length");
	sprintf(tempheader2.value, "%d", response->body->content_length);
	strcpy(tempheader3.key, "Content-Type");
	strcpy(tempheader3.value, get_content_type(request->startline->filename));
	response->header->headers.push_back(tempheader1);
	response->header->headers.push_back(tempheader2);
	response->header->headers.push_back(tempheader3);
	response->header->no = 3;
}

char* get_content_type(char *name) {
    char *dot;
	const char *buf;  
	char* ret = new char[100];
	puts("****************confirm the content type");
	puts(name);
    dot = strrchr(name, '.'); 
	
	if(dot == NULL)
		buf = "text/plain";
	/* Text */
    else if ( strcmp(dot, ".txt") == 0 )
        buf = "text/plain";
	else if ( strcmp( dot, ".css" ) == 0 )
        buf = "text/css";
    else if ( strcmp( dot, ".js" ) == 0 )
        buf = "text/javascript";
    else if ( strcmp(dot, ".xml") == 0 || strcmp(dot, ".xsl") == 0 )
        buf = "text/xml";
    else if ( strcmp(dot, ".xhtm") == 0 || strcmp(dot, ".xhtml") == 0 || strcmp(dot, ".xht") == 0 )
        buf = "application/xhtml+xml";
    else if ( strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0 || strcmp(dot, ".shtml") == 0 || strcmp(dot, ".hts") == 0 )
        buf = "text/html";

	/* Images */
    else if ( strcmp( dot, ".gif" ) == 0 )
        buf = "image/gif";
    else if ( strcmp( dot, ".png" ) == 0 )
        buf = "image/png";
    else if ( strcmp( dot, ".bmp" ) == 0 )
        buf = "application/x-MS-bmp";
    else if ( strcmp( dot, ".jpg" ) == 0 || strcmp( dot, ".jpeg" ) == 0 || strcmp( dot, ".jpe" ) == 0 || strcmp( dot, ".jpz" ) == 0 )
        buf = "image/jpeg";

	/* Audio & Video */
    else if ( strcmp( dot, ".wav" ) == 0 )
        buf = "audio/wav";
    else if ( strcmp( dot, ".wma" ) == 0 )
        buf = "audio/x-ms-wma";
    else if ( strcmp( dot, ".wmv" ) == 0 )
        buf = "audio/x-ms-wmv";
    else if ( strcmp( dot, ".au" ) == 0 || strcmp( dot, ".snd" ) == 0 )
        buf = "audio/basic";
    else if ( strcmp( dot, ".midi" ) == 0 || strcmp( dot, ".mid" ) == 0 )
        buf = "audio/midi";
    else if ( strcmp( dot, ".mp3" ) == 0 || strcmp( dot, ".mp2" ) == 0 )
        buf = "audio/x-mpeg";
	else if ( strcmp( dot, ".rm" ) == 0  || strcmp( dot, ".rmvb" ) == 0 || strcmp( dot, ".rmm" ) == 0 )
        buf = "audio/x-pn-realaudio";
    else if ( strcmp( dot, ".avi" ) == 0 )
        buf = "video/x-msvideo";
    else if ( strcmp( dot, ".3gp" ) == 0 )
        buf = "video/3gpp";
    else if ( strcmp( dot, ".mov" ) == 0 )
        buf = "video/quicktime";
    else if ( strcmp( dot, ".wmx" ) == 0 )
        buf = "video/x-ms-wmx";
	else if ( strcmp( dot, ".asf" ) == 0  || strcmp( dot, ".asx" ) == 0 )
        buf = "video/x-ms-asf";
    else if ( strcmp( dot, ".mp4" ) == 0 || strcmp( dot, ".mpg4" ) == 0 )
        buf = "video/mp4";
	else if ( strcmp( dot, ".mpe" ) == 0  || strcmp( dot, ".mpeg" ) == 0 || strcmp( dot, ".mpg" ) == 0 || strcmp( dot, ".mpga" ) == 0 )
        buf = "video/mpeg";

	/* Documents */
    else if ( strcmp( dot, ".pdf" ) == 0 )
        buf = "application/pdf";
    else if ( strcmp( dot, ".rtf" ) == 0 )
        buf = "application/rtf";
	else if ( strcmp( dot, ".doc" ) == 0  || strcmp( dot, ".dot" ) == 0 )
        buf = "application/msword";
	else if ( strcmp( dot, ".xls" ) == 0  || strcmp( dot, ".xla" ) == 0 )
        buf = "application/msexcel";
	else if ( strcmp( dot, ".hlp" ) == 0  || strcmp( dot, ".chm" ) == 0 )
        buf = "application/mshelp";
	else if ( strcmp( dot, ".swf" ) == 0  || strcmp( dot, ".swfl" ) == 0 || strcmp( dot, ".cab" ) == 0 )
        buf = "application/x-shockwave-flash";
	else if ( strcmp( dot, ".ppt" ) == 0  || strcmp( dot, ".ppz" ) == 0 || strcmp( dot, ".pps" ) == 0 || strcmp( dot, ".pot" ) == 0 )
        buf = "application/mspowerpoint";

	/* Binary & Packages */
    else if ( strcmp( dot, ".zip" ) == 0 )
        buf = "application/zip";
    else if ( strcmp( dot, ".rar" ) == 0 )
        buf = "application/x-rar-compressed";
    else if ( strcmp( dot, ".gz" ) == 0 )
        buf = "application/x-gzip";
    else if ( strcmp( dot, ".jar" ) == 0 )
        buf = "application/java-archive";
	else if ( strcmp( dot, ".tgz" ) == 0  || strcmp( dot, ".tar" ) == 0 )
        buf = "application/x-tar";
	else
		buf = "application/octet-stream";
	strcpy(ret, buf);
	return ret;
}

void handler_not_found(http_request_t *request, http_response_t *response) {

	response->startline->status_code = HTTP_NOT_FOUND;
	strcpy(response->startline->version , HTTP_VERSION);
	response->startline->reason_phase = NOT_FOUND;

	char tempbody[100] = "<html><body> <h1>404</h1>\n\n sorry... file not found </body></html>";
	strcpy(response->body->content, tempbody);
	response->body->content_length = strlen(tempbody);

	http_table_elt_t tempheader1, tempheader2;
	strcpy(tempheader1.key , "Connection");
	strcpy(tempheader1.value , "keep-alive");
	strcpy(tempheader2.key, "Content-length");
	sprintf(tempheader2.value, "%d", response->body->content_length);
	response->header->headers.push_back(tempheader1);
	response->header->headers.push_back(tempheader2);
	response->header->no = 2;
}
void handler_bad_request(http_request_t *request, http_response_t *response) {
}
//TODO 可以考虑给出比较友好的信息 HTML文档 而且具有参考价值
void handler_forbidden(http_request_t *request, http_response_t *response) {

	response->startline->status_code = HTTP_FORBIDDEN;
	strcpy(response->startline->version , HTTP_VERSION);
	response->startline->reason_phase = FORBIDDEN;

	char tempbody[100] = "<html><body> <h1>403</h1>\n\n Forbidden </body></html>";
	strcpy(response->body->content, tempbody);
	response->body->content_length = strlen(tempbody);

	http_table_elt_t tempheader1, tempheader2;
	strcpy(tempheader1.key , "Connection");
	strcpy(tempheader1.value , "keep-alive");
	strcpy(tempheader2.key, "Content-length");
	sprintf(tempheader2.value, "%d", response->body->content_length);
	response->header->headers.push_back(tempheader1);
	response->header->headers.push_back(tempheader2);
	response->header->no = 2;
}

void handler_method_not_allowed(http_request_t *request, http_response_t *response) {
	response->startline->status_code = HTTP_NOT_ALLOWED;
	strcpy(response->startline->version , HTTP_VERSION);
	response->startline->reason_phase = METHOD_NOTALLOWED;

	char tempbody[100] = "<html><body> <h1>405</h1>\n\n METHOD NOT ALLOWED </body></html>";
	strcpy(response->body->content, tempbody);
	response->body->content_length = strlen(tempbody);
	http_table_elt_t tempheader1, tempheader2;
	strcpy(tempheader1.key , "Connection");
	strcpy(tempheader1.value , "keep-alive");
	strcpy(tempheader2.key, "Content-length");
	sprintf(tempheader2.value, "%d", response->body->content_length);
	response->header->headers.push_back(tempheader1);
	response->header->headers.push_back(tempheader2);
	response->header->no = 2;
//	response->header->;//TODO
}

//TODO the details of implemention


//init request & response
bool init_http_request(http_request_t* request) {
//	request = new http_request_t;
	request->startline = init_http_request_startline();
	request->header = init_http_request_header();
	request->body = init_http_request_body();

	//todo
	return true;
}
bool init_http_response(http_response_t* response) {
//	response = new http_response_t;
	response->startline = init_http_response_startline();
	response->header = init_http_response_header();
	response->body = init_http_response_body();

	//todo
	return true;
}

http_request_startline_t * init_http_request_startline() {
	http_request_startline_t * startline = new http_request_startline_t;
	startline->version = new char[50];
	startline->filename = new char[200];

	//todo
	return startline;
}
http_request_header_t *init_http_request_header() {

	http_request_header_t *header = new http_request_header_t;
	header->headers.clear();
	header->no = 0;
	//todo
	return header;
}
http_request_body_t* init_http_request_body() {

	http_request_body_t *body = new http_request_body_t;
	body->content = new char[MAXBUFSIZE];
	body->content_length = 0;
	//todo
	return body;
}
	
	
//init response
http_response_startline_t * init_http_response_startline() {
	http_response_startline_t *startline = new http_response_startline_t;
	startline->version = new char[50];

	//todo
	return startline;
}
http_response_header_t *init_http_response_header() {
	
	http_response_header_t *header = new http_response_header_t;
	header->headers.clear();
	header->no = 0;

	//todo
	return header;
}
http_response_body_t *init_http_response_body() {
	
	http_response_body_t *body = new http_response_body_t;
	body->content = new char[MAXBUFSIZE];
	body->content[0] = '\0';
	body->content_length = 0;

	//todo
	return body;
}

//connection to request
http_method_e get_request_method(char *requestbuf) {
	char method[100];
	sscanf(requestbuf, "%s", method);
//	puts(method);
	if (strcmp(method, "GET") == 0) {
		return GET;
	}else if(strcmp(method, "POST") == 0) {
		return POST;
	}else if(strcmp(method, "DELETE") == 0) {
		return DELETE;
	}else if(strcmp(method, "PUT") == 0) {
		return PUT;
	}else if(strcmp(method, "OPTIONS") == 0) {
		return OPTIONS;
	}else if(strcmp(method, "TRACE") == 0) {
		return TRACE;
	}else {
		return NOT_KNOW;
	}
	
}
//todo more choice and more code
//need filter and judge the content of filename
bool get_request_filename(char *filename, char *requestbuf) {
//	puts(requestbuf);	
	sscanf(requestbuf, "%*s %s", filename);
	return true;
}
//TODO
bool get_request_version(char *version, char *requestbuf) {
	strcpy(version, HTTP_VERSION);
	return true;
}

bool get_request_startline(http_request_startline_t *startline, char *recvbuf) {

	startline->method = get_request_method(recvbuf);
	get_request_version(startline->version, recvbuf);
	get_request_filename(startline->filename, recvbuf);
	return true;
}
//TODO the best method is define yourself parse requestbuf 
//		and not use the C library
bool get_request_header(http_request_header_t* header, char *requestbuf) {
	int &no = header->no;
	char *str_cur = requestbuf;


	return true;
}
bool get_request_body(http_request_body_t *body, char *requestbuf) {
	return true;
}



/*
 *检查数据包的完整行 
 *功能有待完善 
 */
extern "C" int check_complete(char* buf, int len) {
    /*
	 *目前只考虑头部接受完整 (请求中包含/r/n)
	 */
	if (strstr(buf, CHECK_COMPLETE_STRING)) {
		return 1;
	}
	/*
	 *不知道请求体的完整性如何检查
	 *get请求似乎没有请求体 POST有
	 *具体可以抓包(自己写 tcpdump 抓包工具) 浏览器分析
	 */
	return 0;
}

//send response
bool send_response(http_response_t *response) {
	int clientfd = response->clientfd;
	send_response_startline(clientfd, response->startline);
	send_response_header(clientfd, response->header);
	send_response_body(clientfd, response->body);
}
bool send_response_startline(int clientfd, http_response_startline_t *startline) {
	char sendbuf[200];
	sprintf(sendbuf, "%s %d %s\r\n", 
			startline->version,
			startline->status_code,
			reason_entity[startline->reason_phase]
			);	
//	puts(sendbuf);
	int err = send(clientfd, sendbuf, strlen(sendbuf), 0);
	return err == strlen(sendbuf);
}
bool send_response_header(int clientfd, http_response_header_t *header) {
	
	char sendbuf[2000] = "", tempbuf[100];
	int no = header->no;
	for (int i = 0; i < no; i++) {
		sprintf(tempbuf, "%s: %s\r\n", 
				header->headers[i].key,
				header->headers[i].value
				);	
		strcat(sendbuf, tempbuf);
	}
	strcat(sendbuf, "\r\n");
	puts(sendbuf);
	int err = send(clientfd, sendbuf, strlen(sendbuf), 0);
	return err == strlen(sendbuf);
}
//todo 
bool send_response_body(int clientfd, http_response_body_t *body) {
	int err, i, total = 0;
	for (i = 0; i < body->content_length; i += SEND_CHUNK_SIZE) {
		int len = min(SEND_CHUNK_SIZE, body->content_length - i);
		err = send(clientfd, body->content + i, len, 0);
		if (err != len && errno == EAGAIN) {
			perror("send error");
			sleep(1);
			err = send(clientfd, body->content + i, len, 0);
		}
		printf("body send  length %d\n", err);
		total += err;
	}
	printf("body send total length %d\n", total);
//这样不行 只能发送5000多  和MTU有关系估计
	/*err = send(clientfd, body->content, body->content_length, 0);
		if (err != body->content_length)
			perror("send error");
		printf("body send length %d\n", err);
	printf("body send length %d\n", err);
	*/
//	如果出现错误 需要返回到框架 因为头部有cotent-length 但是实际发送没那么大
//	这也是自己的浏览器访问会慢 和 设置超时时间短会加快的原因
//	return total == body->cotent_length
	return true;
}

//destroy request 
bool destroy_http_request(http_request_t *request) {
	destroy_http_request_startline(request->startline);
	destroy_http_request_header(request->header);
	destroy_http_request_body(request->body);
	
}
//destroy request startline
bool destroy_http_request_startline(http_request_startline_t *startline) {
	delete startline->version;
	delete startline->filename;
	delete startline;

}
//destroy request header
bool destroy_http_request_header(http_request_header_t *header) {
	int no = header->no;
	/*
	for (int i = 0; i < no; i++) {
		delete header->headers[i];
	}
	*/
	header->headers.clear();
	delete header;
}
//destroy request body
bool destroy_http_request_body(http_request_body_t *body) {
	delete body->content;
	delete body;
}

//destroy response 
bool destroy_http_response(http_response_t *response) {
	destroy_http_response_startline(response->startline);
	destroy_http_response_header(response->header);
	destroy_http_response_body(response->body);

}
//destroy response startline
bool destroy_http_response_startline(http_response_startline_t *startline) {
	
	delete startline->version;
	delete startline;

}
//destroy response header
bool destroy_http_response_header(http_response_header_t *header) {
	int no = header->no;
	/*
	for (int i = 0; i < no; i++) {
		delete header->headers[i];
	}
	*/
	header->headers.clear();
	delete header;
}
//destroy response body
bool destroy_http_response_body(http_response_body_t *body) {
	delete body->content;
	delete body;
}
