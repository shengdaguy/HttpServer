#ifndef SERVICE_H
#define SERVICE_H

#include "../include/base.h"
#include "../include/connpool.h"
#include "http_data.h"


extern "C"  int check_complete(char* buf, int len);
extern "C" bool handler_request(conn* connection);


//init request 
bool init_http_request(http_request_t*);
http_request_startline_t * init_http_request_startline();
http_request_header_t * init_http_request_header();
http_request_body_t * init_http_request_body();
//init response
bool init_http_response(http_response_t*);
http_response_startline_t* init_http_response_startline();
http_response_header_t* init_http_response_header();
http_response_body_t* init_http_response_body();

char* get_content_type(char* filename);

bool parse_request(http_request_t* request, http_response_t* response);
bool conn_to_request(conn* connection, http_request_t* request);


//connection to request
http_method_e get_request_method(char *requestbuf);
bool get_request_filename(char *filename, char *requestbuf);
bool get_request_version(char *version, char *requestbuf);

bool get_request_startline(http_request_startline_t *startline, char *recvbuf);
bool get_request_header(http_request_header_t* header, char *requestbuf);
bool get_request_body(http_request_body_t *body, char *requestbuf);


//handler by method
void handler_get(http_request_t *request, http_response_t *response);
void handler_post(http_request_t *request, http_response_t *response);
void handler_put(http_request_t *request, http_response_t *response);
void handler_delete(http_request_t *request, http_response_t *response);
void handler_head(http_request_t *request, http_response_t *response);
void handler_trace(http_request_t *request, http_response_t *response);
void handler_options(http_request_t *request, http_response_t *response);

//handler by result
void handler_ok(http_request_t *request, http_response_t *response);
void handler_not_found(http_request_t *request, http_response_t *response);
void handler_bad_request(http_request_t *request, http_response_t *response);
void handler_forbidden(http_request_t *request, http_response_t *response);
void handler_method_not_allowed(http_request_t *request, http_response_t *response);

//TODO the details of implemention



//send response 
bool send_response(http_response_t *response);
bool send_response_startline(int clientfd, http_response_startline_t *startline);
bool send_response_header(int clientfd, http_response_header_t *header);
bool send_response_body(int clientfd, http_response_body_t *body);


//destroy request & request
bool destroy_http_request(http_request_t *request);
bool destroy_http_request_startline(http_request_startline_t *startline);
bool destroy_http_request_header(http_request_header_t *header);
bool destroy_http_request_body(http_request_body_t *body);

bool destroy_http_response(http_response_t *response);
bool destroy_http_response_startline(http_response_startline_t *startline);
bool destroy_http_response_header(http_response_header_t *header);
bool destroy_http_response_body(http_response_body_t *body);

#endif
