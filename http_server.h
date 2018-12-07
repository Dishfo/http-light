/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   http_server.h
 * Author: dishfo
 *
 * Created on 2018年12月5日, 下午11:44
 */

#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <sys/socket.h>
#include <sys/select.h>
#include <http_parser.h>
#include <pthread.h>
#include "http_light.h"
#include "string_map.h"

#define MAX_ACP_QUEUE 1024

typedef struct handler_list handler_list;
typedef struct http_server http_server;
typedef void* (*req_handler)(http_requset* req, http_response *resp);

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

#define INVAIL_SERVER -1
#define INVAILD_ADDR -2

#define MAX_REQ_BUF 8193

struct handler_list {
    char* url;
    req_handler handler;
    struct handler_list* next;
};

struct http_server {
    struct sockaddr *saddr;
    http_parser_settings req_settings;
    int sockfd;
    fd_set rset;
    /**
     * 
     * 使用一个特定的结构
     * 根据url长度预先选择最长的字符串
     * 
     */
    handler_list *handlers;
    pthread_mutex_t mutex;
    int *clients;
    int stop;
    int connum;
};

extern http_server* create_server(int port, const char* addr);
extern int start_server(http_server* server);
extern void add_handler(http_server* server, const char* url,
        req_handler handler);
extern void close_server(http_server* server);
extern void free_handlers(handler_list* head);

#endif /* HTTP_SERVER_H */






