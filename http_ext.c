/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stddef.h>

#include "http_ext.h"
#include "http_light.h"

int max_field_num = 19;
char *head_fields[] = {
    "cache_control",
    "connection",
    "upgrade",
    "accept",
    "accept-charset",
    "accept-encoding",
    "accept-language",
    "authorization",
    "host",
    "user-agent",
    "max-forwards",
    "server",
    "location",
    "content-encoding",
    "content-language",
    "content-length",
    "content-location",
    "content-type",
    "set-cookie",
    "cookie"
};

const char* conn_close = "close";
const char* conn_keep = "keep-alive";

int version_type = 2;
char* http_version_strs[] = {
    "HTTP/1.0",
    "HTTP/1.1",
    "HTTP/2.0"
};

const char* http_version_str(enum http_version version) {
    if (version > 2) {
        return NULL;
    }
    return http_version_strs[version];
}

const char* http_head_field(enum http_head_field field) {
    if (field > 19) {
        return NULL;
    }
    return head_fields[field];
}

int verion_aton(char* v, size_t len) {
    for (int i = 0; i <= version_type; i++) {
        if (strncmp(http_version_strs[i], v, len) == 0) {
            return i;
        }
    }
    return unknow;
}