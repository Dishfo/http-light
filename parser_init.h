/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <http_parser.h>
#include <stdlib.h>

#include "http_light.h"

char *method_map[] = {"DELETE", "GET",
    "HEAD", "POST", "PUT",
    "CONNECT", "OPTIONS", "TRACE"};

typedef struct custom_data custom_data;

int requset_on_url(http_parser* parser, const char* at, size_t len);
int requset_on_headers_compete(http_parser* parser);
int request_on_field(http_parser* parser, const char* at, size_t len);
int request_on_value(http_parser* parser, const char* at, size_t len);

int response_on_status(http_parser* parser, const char* at, size_t len);
int response_on_headers_compete(http_parser* parser);
int response_on_field(http_parser* parser, const char* at, size_t len);
int response_on_value(http_parser* parser, const char* at, size_t len);

struct custom_data {
    char headbuf[30];
    char valuebuf[1024];
    http_requset* req;
    http_response* resp;
};

http_parser *req_parser = NULL;
http_parser *resp_parser = NULL;

http_parser_settings req_settings;
http_parser_settings resp_settings;

int requset_on_headers_compete(http_parser* parser) {
    custom_data* data = parser->data;
    return 0;
}

int requset_on_url(http_parser* parser, const char* at, size_t len) {
    custom_data* data = parser->data;
    char* url = malloc(len + 1);
    strncpy(url, at, len);
    url[len] = 0;
    printf("url is %s\n", url);
    struct http_parser_url* urlinfo =
            malloc(sizeof (struct http_parser_url));
    http_parser_url_init(urlinfo);
    http_parser_parse_url(url, len, 0, urlinfo);
    set_request_url(data->req, url);
    printf(" %d %s\n",urlinfo->field_data[UF_QUERY].off,url+30);
    at += (len + 1);
    char *vend = strstr(at, "\r\n");
    int vlen = vend - at;
    strncpy(url, at, vlen);
    url[vlen] = 0;


    free(url);
    return 0;
}

int request_on_field(http_parser* parser, const char* at, size_t len) {
    custom_data* data = parser->data;
    strncpy(data->headbuf, at, len);
    data->headbuf[len] = 0;
    return 0;
}

int request_on_value(http_parser* parser, const char* at, size_t len) {
    custom_data* data = parser->data;
    strncpy(data->valuebuf, at, len);
    data->valuebuf[len] = 0;
    request_set_header(data->req, data->headbuf, data->valuebuf);
    return 0;
}

int response_on_status(http_parser* parser, const char* at, size_t len) {
    custom_data *data = parser->data;
    char status[len + 1];
    strncpy(status, at, len);
    status[len] = 0;
    int s = atoi(status);
    data->resp->status = s;
    return 0;
}

int response_on_headers_compete(http_parser* parser) {
    custom_data *data = parser->data;

    return 0;
}

int response_on_field(http_parser* parser, const char* at, size_t len) {

    return 0;
}

int response_on_value(http_parser* parser, const char* at, size_t len) {

    return 0;
}

void init_context() {
    req_parser = malloc(sizeof (http_parser));
    http_parser_init(req_parser, HTTP_REQUEST);
    custom_data *data = malloc(sizeof (custom_data));
    req_parser->data = data;
    data->req = new_http_request();
    req_settings.on_header_field = request_on_field;
    req_settings.on_header_value = request_on_value;
    req_settings.on_headers_complete = requset_on_headers_compete;
    req_settings.on_url = requset_on_url;
    resp_parser = malloc(sizeof (http_parser));
    http_parser_init(resp_parser, HTTP_RESPONSE);

}

void release_context() {


    free(req_parser);
    free(resp_parser);
}




