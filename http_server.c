/*  
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include "unp.h"
#include <string.h>
#include "http_server.h"

typedef struct http_data http_data;

struct http_data {
    void *data;
    size_t len;
    http_server *server;
    http_requset* req;
    http_response* resp;
    int connfd;
    char *field;
    size_t flen;
    int index;
};

void* handle_raw(void* data);
static void add_handler1(handler_list* head, const char* url, req_handler h);
static void not_found_handler(http_requset *req, http_response *resp);
static void invaild_req_handler(http_requset *req, http_response *resp);
static void free_http_data(http_data* data);

static struct sockaddr_storage *get_addr(int port, const char* addr);
static int get_socket(struct sockaddr* addr);
static req_handler match_handler(http_server* server, char* url);
static int url_match(char* url, char* tom);
static http_data* new_http_data();

//报文处理相关函数
static int configclose(http_response* resp);
static void revise_length(http_response* resp);
static int req_on_message_compete(http_parser* parser);
static int req_on_url(http_parser* parser, const char *at, size_t length);
static int req_on_field(http_parser* parser, const char *at, size_t length);
static int req_on_value(http_parser* parser, const char *at, size_t length);

http_server* create_server(int port, const char* addr) {

    http_server* server = malloc(sizeof (http_server));
    server->saddr = (struct sockaddr*) get_addr(port, addr);
    server->clients = malloc(FD_SETSIZE * sizeof (int));
    server->connum = 0;
    pthread_mutex_t init = PTHREAD_MUTEX_INITIALIZER;
    server->mutex = init;
    for (int i = 0; i < FD_SETSIZE; i++) {
        server->clients[i] = -1;
    }

    server->req_settings.on_message_complete = req_on_message_compete;
    server->req_settings.on_url = req_on_url;
    server->req_settings.on_header_field = req_on_field;
    server->req_settings.on_header_value = req_on_value;

    server->handlers = malloc(sizeof (handler_list));
    memset(server->handlers, 0, sizeof (handler_list));
    return server;
}

int start_server(http_server* server) {
    int maxfd;
    int maxi = 0;
    int actn;
    if (server == NULL) {
        return INVAIL_SERVER;
    }
    struct sockaddr* saddr = server->saddr;
    size_t addr_len;
    if (saddr == NULL) {
        return INVAIL_SERVER;
    }

    if (saddr->sa_family = AF_INET) {
        addr_len = sizeof (struct sockaddr_in);
    } else if (saddr->sa_family = AF_INET6) {
        addr_len = sizeof (struct sockaddr_in6);
    } else {
        return INVAILD_ADDR;
    }

    server->sockfd = Socket(saddr->sa_family, SOCK_STREAM, 0);
    Bind(server->sockfd, saddr, addr_len);
    Listen(server->sockfd, MAX_ACP_QUEUE);
    fd_set *rset = &(server->rset);
    int * clients = server->clients;
    int listenfd = server->sockfd;


    FD_ZERO(rset);
    maxfd = server->sockfd + 1;
    FD_SET(server->sockfd, rset);

    while (server->stop == 0) {
        int i;
        int n = Select(maxfd, rset, NULL, NULL, NULL);
        if (n < 0) {
            printf("select %s\n", strerror(errno));
            break;
        }


        if (FD_ISSET(listenfd, rset)) {
            printf("accepting \n");
            int acpfd = Accept(listenfd, NULL, NULL);
            printf("establish a new connection\n");
            pthread_mutex_lock(&(server->mutex));
            for (i = 1; i < FD_SETSIZE; i++) {
                if (clients[i] == -1) {
                    clients[i] = acpfd;
                    if (acpfd >= maxfd) {
                        maxfd = acpfd + 1;
                    }
                    if (i > maxi) {
                        maxi = i;
                    }
                    break;
                }
            }
            if (i == FD_SETSIZE) {
                printf("to mush tcp connection");
            } else {
                FD_SET(acpfd, rset);
            }
            FD_SET(listenfd, rset);
            pthread_mutex_unlock(&(server->mutex));
        }

        printf("start process client connection\n");
        for (i = 1; i <= maxi; i++) {
            pthread_mutex_lock(&server->mutex);
            if (clients[i] != -1) {
                if (FD_ISSET(clients[i], rset)) {
                    char* reqbuf = calloc(MAX_REQ_BUF, sizeof (char));
                    int nread = Read(clients[i], reqbuf, MAX_REQ_BUF);
                    if (nread <= 0) {
                        printf("on read %s\n", strerror(errno));
                        close(clients[i]);
                        clients[i] = -1;
                        printf("read close \n");
                        goto next;
                    }
                    if (nread == 8193) {
                        close(clients[i]);
                        clients[i] = -1;
                        goto next;
                    }

                    http_data* hdata = new_http_data();
                    hdata->data = reqbuf;
                    hdata->connfd = clients[i];
                    hdata->len = nread;
                    hdata->index = i;
                    hdata->server = server;
                    pthread_t tid;
                    pthread_create(&tid, NULL, handle_raw, hdata);
                }
            }
next:
            pthread_mutex_unlock(&server->mutex);
        }

    }

    sleep(5);
    close_server(server);
}

void add_handler(http_server* server, const char* url,
        req_handler handler) {
    if (url == NULL || handler == NULL || server == NULL) {
        return;
    }
    add_handler1(server->handlers, url, handler);
}

void close_server(http_server* server) {
    if (server->clients != NULL) {
        for (int i = 0; i < FD_SETSIZE; i++) {
            if (server->clients[i] != -1) {
                close(server->clients[i]);
            }
        }
    }
    free(server->saddr);
    free_handlers(server->handlers);
    close(server->sockfd);
    free(server->clients);
}

static struct sockaddr_storage *get_addr(int port, const char* addr) {
    int n;
    int af;
    struct sockaddr_storage * saddr =
            malloc(sizeof (struct sockaddr_storage));

    char* addrbuf = malloc(sizeof (struct in6_addr));
    n = inet_pton(AF_INET, addr, addrbuf);
    af = AF_INET;
    if (n == 0) {
        n = inet_pton(AF_INET6, addr, addrbuf);
        af = AF_INET6;
    }

    if (n == 0) {
        free(saddr);
        return NULL;
    }

    saddr->ss_family = af;
    if (af == AF_INET) {
        struct sockaddr_in *sin = (struct sockaddr_in*) saddr;
        sin->sin_addr = *((struct in_addr*) addrbuf);
        sin->sin_port = htons(port);
    } else {
        struct sockaddr_in6 *sin6 = (struct sockaddr_in6*) saddr;
        sin6->sin6_addr = *((struct in6_addr*) addrbuf);
        sin6->sin6_port = htons(port);

    }
    return saddr;
}

static http_data* new_http_data() {

    http_data *data = malloc(sizeof (http_data));
    data->req = new_http_request();
    data->req->method = -1;
    data->resp = new_http_response();
    data->field = malloc(20);
    data->flen = 20;
    return data;
}

void* handle_raw(void* data) {
    int needclose = 0;
    http_data* hdata = data;
    char* outbuf = NULL;
    int n;
    http_server* server = hdata->server;

    http_parser_settings* settings = &(hdata->server->req_settings);
    http_parser *parser = malloc(sizeof (http_parser));
    http_parser_init(parser, HTTP_REQUEST);

    parser->data = data;
    http_parser_execute(parser, settings, hdata->data, hdata->len);

    if (hdata->req->method == -1) {
        needclose = 1;
        goto end;
    }

    req_handler handler = match_handler(hdata->server, hdata->req->url);
    if (handler != NULL) {
        handler(hdata->req, hdata->resp);
    } else {
        not_found_handler(hdata->req, hdata->resp);
    }
    /**
     * 无效的报文操作
     * @param url
     * @return 
     */
    const char* conl_field = http_head_field(HTTP_FIELD_CONTENT_LENGTH);
    const char *len_str = response_get_header(hdata->resp,
            conl_field);
    if (len_str == NULL) {
        revise_length(hdata->resp);
        char tmpstr[15];
        memset(tmpstr, 0, 15);
        snprintf(tmpstr, 14, "%d", (int) hdata->resp->data_len);
        response_set_header(hdata->resp, conl_field, tmpstr);
    }
    needclose=configclose(hdata->resp);
out:
    outbuf = malloc(1024 * 1024);
    n = squash_http_response(hdata->resp, outbuf, 1024 * 1024);

    if (n > 0) {
        write(hdata->connfd, outbuf, n);
        printf("%s \n", outbuf);
    } else {
        //非法的结构处理
        needclose = 1;
    }
end:
    pthread_mutex_lock(&server->mutex);
    if (needclose) {
        close(hdata->connfd);
        server->clients[hdata->index] = -1;
    } else {
        FD_SET(hdata->connfd, &server->rset);
    }
    pthread_mutex_unlock(&server->mutex);

    free(parser);
    free_http_data(hdata);
    free(outbuf);
    printf("all is done\n");
}

void free_handlers(handler_list* head) {
    handler_list* ptr = head;
    while (ptr != NULL) {
        handler_list* tmp = ptr;
        ptr = ptr->next;
        free(tmp->url);
        free(tmp);
    }
}

static req_handler match_handler(http_server* server, char* url) {
    handler_list * ptr = server->handlers->next;
    while (ptr != NULL) {
        if (url_match(ptr->url, url)) {
            break;
        }
        ptr = ptr->next;
    }

    if (ptr != NULL) {
        return ptr->handler;
    }
    return NULL;
}

static int url_match(char* url, char* tom) {
    char *s1 = url;
    char *s2 = tom;

    while (*s1 != '\0' && *s2 != '\0') {
        if (*s1 == '*') {
            if (*(s1 + 1) == '\0') {
                s1++;
                break;
            } else {
                if (*(s1 + 1) == *s2) {
                    s1++;
                }
                s2++;
            }
        } else {
            if (*s1 != *s2) {
                break;
            }
            s1++;
            s2++;
        }
    }

    if (*s1 == '\0') {
        return 1;
    }

    return 0;
}

static void not_found_handler(http_requset *req, http_response *resp) {
    resp->status = HTTP_STATUS_NOT_FOUND;
    resp->version = req->version;
    resp->data = NULL;
    resp->data_len = 0;
}

static void invaild_req_handler(http_requset *req, http_response *resp) {

}

static void add_handler1(handler_list* head, const char* url, req_handler h) {
    handler_list *ptr = head->next;
    handler_list* pre = head;
    int l = strlen(url);
    while (ptr != NULL) {
        if (strlen(url) >= strlen(ptr->url)) {
            break;
        }
        pre = ptr;
        ptr = ptr->next;
    }

    if (ptr == NULL) {
        pre->next = malloc(sizeof (handler_list));
        pre->next->next = NULL;
        pre->next->handler = h;
        pre->next->url = malloc(l + 1);
        strncpy(pre->next->url, url, l);
        pre->next->url[l] = 0;
    } else {
        handler_list* tmp = pre->next;
        pre->next = malloc(sizeof (handler_list));
        pre->next->next = tmp;
        pre->next->handler = h;
        pre->next->url = malloc(l + 1);
        strncpy(pre->next->url, url, l);
        pre->next->url[l] = 0;

    }
}

/**
 * 
 * raw 数据提相关函数
 * 
 *  
 */

static int configclose(http_response* resp) {
    const char* conn = response_get_header(resp,
            http_head_field(HTTP_FIELD_CONNECTION));
    if (conn != NULL && strcasecmp(conn,conn_keep) == 0) {
        return 0;
    }
    return 1;
}

static void revise_length(http_response* resp) {
    if (resp->data == NULL) {
        resp->data_len = 0;
    }
}

static int req_on_message_compete(http_parser* parser) {
    http_data *data = parser->data;
    http_requset* req = data->req;
    req->method = parser->method;

    return 0;
}

static int req_on_url(http_parser* parser, const char *at,
        size_t length) {
    int res = 0;
    struct http_parser_url urlset;
    http_data *data = parser->data;
    http_requset* req = data->req;

    req->uri = malloc(length + 1);
    strncpy(req->uri, at, length);
    req->uri[length] = 0;

    res = http_parser_parse_url(at, length, 0, &urlset);
    if (res == 0) {
        char* path = (char*) at + urlset.field_data[UF_PATH].off;
        int pl = urlset.field_data[UF_PATH].len;
        req->url = malloc(pl + 1);
        strncpy(req->url, path, pl);
        req->url[pl] = 0;
    } else {
        return HPE_CB_url;
    }

    return 0;
}

static int req_on_field(http_parser* parser, const char *at,
        size_t length) {
    http_data *data = parser->data;
    if (length + 1 > data->flen) {
        free(data->field);
        data->field = malloc(length + 1);
        data->flen = length + 1;
    }
    strncpy(data->field, at, length);
    data->field[length] = 0;
    return 0;
}

static int req_on_value(http_parser* parser, const char *at,
        size_t length) {
    http_data *data = parser->data;
    http_requset* req = data->req;

    char * tmp = malloc(length + 1);
    strncpy(tmp, at, length);
    tmp[length] = 0;
    request_set_header(req, data->field, tmp);
    free(tmp);
    return 0;
}

void free_http_data(http_data* data) {
    free(data->data);
    free_request(data->req);
    free_response(data->resp);
    free(data);
}