#ifndef DATA_H

#define DATA_H
//todo 完善结构和设计
//2XX状态码
#define HTTP_OK                        200
#define HTTP_CREATED                   201
#define HTTP_ACCEPTED                  202
#define HTTP_NO_CONTENT                204
#define HTTP_PARTIAL_CONTENT           206
//3XX状态码
#define HTTP_SPECIAL_RESPONSE          300
#define HTTP_MOVED_PERMANENTLY         301
#define HTTP_MOVED_TEMPORARILY         302
#define HTTP_SEE_OTHER                 303
#define HTTP_NOT_MODIFIED              304
#define HTTP_TEMPORARY_REDIRECT        307
//4XX状态码
#define HTTP_BAD_REQUEST               400
#define HTTP_UNAUTHORIZED              401
#define HTTP_FORBIDDEN                 403
#define HTTP_NOT_FOUND                 404
#define HTTP_NOT_ALLOWED               405
#define HTTP_REQUEST_TIME_OUT          408
#define HTTP_CONFLICT                  409
#define HTTP_LENGTH_REQUIRED           411
#define HTTP_PRECONDITION_FAILED       412
#define HTTP_REQUEST_ENTITY_TOO_LARGE  413
#define HTTP_REQUEST_URI_TOO_LARGE     414
#define HTTP_UNSUPPORTED_MEDIA_TYPE    415
#define HTTP_RANGE_NOT_SATISFIABLE     416



//5XX状态码
#define HTTP_INTERNAL_SERVER_ERROR     500
#define HTTP_NOT_IMPLEMENTED           501
#define HTTP_BAD_GATEWAY               502
#define HTTP_SERVICE_UNAVAILABLE       503
#define HTTP_GATEWAY_TIME_OUT          504
#define HTTP_INSUFFICIENT_STORAGE      507

enum http_method_e {
	GET = 0,
	POST,
	DELETE,
	PUT,
	HEAD,
	OPTIONS,
	TRACE,
	NOT_KNOW,
};


enum http_reason_phrase_e{
	OK = 0,
	NOT_FOUND,
	BAD_REQUEST,
	FORBIDDEN,
	METHOD_NOTALLOWED,
	REQUEST_ENTITYTOOLONG,
	NOT_IMPLEMENTED,
};
//todo need better design 
char reason_entity[20][100] = {
	"OK",
   	"Not Found",
   	"Bad Request",
	"Forbidden",
   	"Method Not allowed",
   	"Request entity too long",
	"Not Implemented"
};

typedef struct {

	char key[100];
	char value[100];

}http_table_elt_t;
/*
 *the http request header
 */
typedef struct {
	//TODO
	vector<http_table_elt_t> headers;
	int no;
}http_request_header_t;

/*
 *the http response header
 */
typedef struct {
	//TODO
//	http_table_elt_t *list;//以NULL结尾的链表
	vector<http_table_elt_t> headers;
	int no;
}http_response_header_t;

/*
 *the http request body
 */
typedef struct {
	char *content;
	int content_length;
}http_request_body_t;


typedef struct {
	http_method_e method;//enum;
	char *version;
	char *filename;
}http_request_startline_t;


typedef struct {
	int status_code;
	char *version;
	http_reason_phrase_e reason_phase;
}http_response_startline_t;
/*
 *the http response body
 *the design is so poor, need cache(use block list).
 */
typedef struct {
	char *content;
	int content_length;
}http_response_body_t;
/*
 *the type of http request
 */
typedef struct {
	int clientfd;
	http_request_startline_t *startline;
	http_request_header_t *header;
	http_request_body_t *body;
}http_request_t;

typedef struct {
	int clientfd;
	http_response_startline_t *startline;
	http_response_header_t *header;
	http_response_body_t *body;
}http_response_t;

#endif
