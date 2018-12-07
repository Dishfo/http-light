/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   http_light.h
 * Author: dishfo
 *
 * Created on 2018年12月4日, 上午10:18
 */

#ifndef HTTP_LIGHT_H
#define HTTP_LIGHT_H

#include "string_map.h"
#include "http_ext.h"
#include <string.h>
#include <stdio.h>
#include <http_parser.h>


#define MAX_MSG_LEN (15-1)
#define MAX_VERSIOON_LEN (10-1)
#define MAX_METHOD_LEN (10-1)

#define APPEND_STR(dst,src,n) strncpy(dst,src,n) ;\
dst+=n 
 
#define APPEND_SPC(str) *str++=' '\
 
#define APPEND_CTLR(str) *str++='\r';\
*str++='\n'
 
#define APPEND_COLON(str) *str++=':';\
 APPEND_SPC(str)

#define BUF_BOUND_CHECK(size,need) \
if(size < (need)){\
      return -1; \
}

typedef struct http_requset http_requset;
typedef struct http_response http_response;
typedef struct http_headers http_headers;
typedef struct http_pair http_pair;

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

struct http_pair {
    char* name;
    char* val;
    http_pair* next;
};

struct http_headers {
    http_pair *head;
    http_pair **tail;
};

struct http_requset {
    enum http_version version;
    enum http_method method;
    char *url;
    char *uri;
    str_map *headers;
    void *data;
    size_t data_len;
};

/**
 * 考慮把version msg  method 這樣的值被規定爲常量de
 * 成員設置爲int型數據 使用静态全局的表进行映射
 * 
 * 
 */
struct http_response {
    enum http_version version;
    enum http_status status;
    http_headers headers;
    void *data;
    size_t data_len;
};

extern int request_set_header(http_requset* req,
        const char* field,
        const char*val);

extern int response_set_header(http_response* resp,
        const char* field,
        const char*val);

extern const char* request_get_header(http_requset* req,
        const char* field);

extern const char* response_get_header(http_response *resp,
        const char* field);

extern http_requset* new_http_request();
extern http_response* new_http_response();
extern void set_request_url(http_requset* req, const char* url);
extern int squash_http_response(http_response* resp, void* buf, size_t len);

extern void free_request(http_requset* req);
extern void free_response(http_response* resp);
extern void free_http_headers(http_headers* headers);


#endif /* HTTP_LIGHT_H */

