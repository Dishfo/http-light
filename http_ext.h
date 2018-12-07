/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
  
/* 
 * File:   http_ext.h
 * Author: dishfo
 *
 * Created on 2018年12月5日, 上午9:48
 */
    
#ifndef HTTP_EXT_H
#define HTTP_EXT_H

#include <stdio.h>

  
#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
} 
#endif

#define HTTP_HEAD(xxx) \
xxx(0,CACHE_CONTROL) \
xxx(1,CONNECTION) \
xxx(2,UPGRADE) \
xxx(3,ACCEPT) \
xxx(4,ACCEPT_CHARSET) \
xxx(5,ACCEPT_ENCODING) \
xxx(6,ACCEPT_LANGUAGE) \
xxx(7,AUTHORIZATION) \
xxx(8,HOST) \
xxx(9,USER_AGENT) \
xxx(10,MAX_FORWARDS) \
xxx(11,SERVER) \
xxx(12,LOCATION) \
xxx(13,CONTENT_ENCODING) \
xxx(14,CONTENT_LANGUAGE) \
xxx(15,CONTENT_LENGTH) \
xxx(16,CONTENT_LOCATION) \
xxx(17,CONTENT_TYPE) \
xxx(18,SET_COOKIE) \
xxx(19,COOKIE) 
 
#define HTTP_FIELD(n,xxx) HTTP_FIELD_##xxx=n,
enum http_head_field{
    
    HTTP_HEAD(HTTP_FIELD)
};

enum http_version{
    http_10=0,
    http_11=1,
    http_2=2,
    unknow,
};

extern const char* conn_close;
extern const char* conn_keep;

extern const char* http_version_str(enum http_version version);
extern const char* http_head_field(enum http_head_field field);  
extern int verion_aton(char* v,size_t len);
#endif /* HTTP_EXT_H */
  
  