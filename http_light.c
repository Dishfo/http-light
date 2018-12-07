/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdlib.h>
#include <http_parser.h>
#include "http_light.h"

static int add_header_pair(http_headers* headers,
        const char* field,
        const char *val);
static char* get_header_value(http_headers* headers,
        const char* field);

int request_set_header(http_requset* req,
        const char* field,
        const char*val) {

    if (req == NULL || req->headers == NULL) {
        return -1;
    }

    if (field == NULL || val == NULL) {
        return -2;
    }
    char *old = map_put(req->headers, field, val, strlen(val) + 1, SET_NEWVAL);
    free(old);
    return 0;
}

int response_set_header(http_response* resp,
        const char* field,
        const char*val) {

    if (resp == NULL) {
        return -4;
    }

    if (field == NULL || val == NULL) {
        return -2;
    }

    add_header_pair(&(resp->headers), field, val);
    return 0;
}

const char* request_get_header(http_requset* req,
        const char* field) {
    if (req == NULL || req->headers == NULL || field == NULL) {
        return NULL;
    }
    size_t len;
    return map_get(req->headers, field, &len);
}

const char* response_get_header(http_response *resp,
        const char* field) {

    if (resp == NULL || field == NULL) {
        return NULL;
    }

    return get_header_value(&resp->headers, field);
}

void set_request_url(http_requset* req, const char* url) {
    if (req == NULL || url == NULL) {
        return;
    }
    int n = strlen(url);
    req->url = calloc(strlen(url) + 1, sizeof (char));
    strcpy(req->url, url);
    req->url[n] = 0;
}

http_requset* new_http_request() {
    http_requset* req = calloc(1, sizeof (http_requset));
    req->headers = malloc(sizeof (str_map));
    req->headers->table = NULL;
    map_init(req->headers);
    return req;
}

http_response* new_http_response() {
    http_response* resp = calloc(1, sizeof (http_response));
    http_pair *pair = calloc(1, sizeof (http_pair));
    resp->headers.head = pair;
    resp->headers.tail = &pair->next;
    return resp;
}

int squash_http_response(http_response* resp,
        void* buf, size_t bufsize) {
    if (resp == NULL || buf == NULL || bufsize <= 0) {
        return -1;
    }
    if (resp->data_len < 0) {
        return -1;
    }

    const char*msg = http_status_str(resp->status);
    const char*version = http_version_str(resp->version);



    int used = 0;
    char status[10];
    snprintf(status, 10, "%d", resp->status);
    char* sndbuf = buf;
    int version_len = strlen(version);
    int status_len = strlen(status);
    int msg_len = strlen(msg);

    BUF_BOUND_CHECK(bufsize, used + version_len + status_len + msg_len + 4)

    APPEND_STR(sndbuf, version, version_len);
    APPEND_SPC(sndbuf);
    APPEND_STR(sndbuf, status, status_len);
    APPEND_SPC(sndbuf);
    APPEND_STR(sndbuf, msg, msg_len);
    APPEND_CTLR(sndbuf);

    used += (version_len + status_len + msg_len + 4);
    struct http_pair * hp = resp->headers.head->next;
    while (hp != NULL) {
        int name_len = strlen(hp->name);
        int val_len = strlen(hp->val);
        BUF_BOUND_CHECK(bufsize, used + name_len + val_len + 4)
        APPEND_STR(sndbuf, hp->name, name_len);
        APPEND_COLON(sndbuf);
        APPEND_STR(sndbuf, hp->val, val_len);
        APPEND_CTLR(sndbuf);
        used += (name_len + val_len + 4);
        hp = hp->next;
    }

    BUF_BOUND_CHECK(bufsize, used + 2)
    APPEND_CTLR(sndbuf);
    used += 2;

    if (resp->data == NULL) {
        if (resp->data > 0) {
            return -1;
        } else {
            return used;
        }
    }

    BUF_BOUND_CHECK(bufsize, used + resp->data_len)
    memcpy(sndbuf, resp->data, resp->data_len);
    sndbuf += resp->data_len;
    APPEND_CTLR(sndbuf);
    used += resp->data_len + 2;
    return used;
}

static int add_header_pair(http_headers* headers,
        const char* field,
        const char *val) {
    if (headers == NULL || headers->head == NULL || headers->tail == NULL) {
        return -3;
    }

    if (field == NULL || val == NULL) {
        return -2;
    }

    http_pair ** tail = headers->tail;
    http_pair *newpair = malloc(sizeof (http_pair));
    newpair->next = NULL;
    newpair->name = calloc(strlen(field) + 1, sizeof (char));
    newpair->val = calloc(strlen(val) + 1, sizeof (char));

    strcpy(newpair->name, field);
    strcpy(newpair->val, val);
    *tail = newpair;
    headers->tail = &newpair->next;
    return 0;
}

static char* get_header_value(http_headers* headers, const char* field) {
    if (headers == NULL || headers->head == NULL || field == NULL) {
        return NULL;
    }

    http_pair* cur = headers->head->next;
    while (cur != NULL) {
        if (strcmp(cur->name, field) == 0) {
            break;
        }
        cur = cur->next;
    }
    if (cur != NULL) {
        return cur->val;

    } else {
        return NULL;
    }
}

void free_response(http_response* resp) {
    free(resp->data);
    free_http_headers(&resp->headers);
    free(resp);
}

void free_request(http_requset* req) {
    free(req->data);
    free_map(req->headers);
    free(req->uri);
    free(req->url);
}

void free_http_headers(http_headers* headers) {
    http_pair* tmp;
    http_pair* ptr=headers->head;
    while (ptr != NULL) {
        tmp = ptr;
        ptr=ptr->next;
        free(tmp->val);
        free(tmp);
    }
}