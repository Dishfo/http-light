/*  
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*  
 * File:   main.c
 * Author: dishfo
 *  
 * Created on 2018年12月4日, 上午10:16
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http_light.h"
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "parser_init.h"
#include "http_server.h"

char filebuf[4096];

char* req_text = "/home/dishfo/test2/req_head";
char* resp_text = "/home/dishfo/test2/http_head";

void transfer(str_map* map) {
    if (map == NULL) {
        return;
    }
    for (int i = 0; i < map->cap; i++) {
        map_bucket bkt = map->table[i];
        printf("\tbucket:  %p %d \n", bkt.head, bkt.size);
        map_node* cur = bkt.head->next;
        while (cur != NULL) {
            printf("\t\tnode-> %s %s\n", cur->name,
                    (char*) cur->val);
            cur = cur->next;
        }

    }

    printf("map %d", map->size);
}
void test_requset_func();
void test_response_func();

void test_requset_func1();
void test_response_func1();

void *base_hand(http_requset*req, http_response* resp) {
    printf("url is %s %s\n", req->uri, http_method_str(req->method));
    response_set_header(resp, "content-length", "0");
    response_set_header(resp, "server", "http_light-0.0.1");
    resp->status = HTTP_STATUS_OK;
    resp->version = http_11;
}

void *specfic_hand(http_requset*req, http_response* resp) {
    printf("url is %s %s\n", req->uri, http_method_str(req->method));
    printf("this is a excat \n");
    resp->data = malloc(15);
    resp->data_len = 14;
    strcpy(resp->data, "12345646");
    response_set_header(resp, "server", "http_light-0.0.1");
    resp->status = HTTP_STATUS_OK;
    resp->version = http_11;
}

/*  
 *  
 */
int main(int argc, char** argv) {
    int port = atoi(argv[1]);
    http_server* server = create_server(port, "127.0.0.1");
    add_handler(server, "/*", base_hand);
    add_handler(server, "/index.html", specfic_hand);

    printf("end\n");
    start_server(server);
    return (EXIT_SUCCESS);
}

void test_requset_func() {
    http_requset * req = new_http_request();
    set_request_url(req, "http://localhost:8080/index.html");

    request_set_header(req, "content-length", "125");
    request_set_header(req, "content-encoding", "gzip");
    transfer(req->headers);
    printf("test request end\n");
}

void test_response_func() {
    http_response * resp = new_http_response();
    resp->version = http_10;
    resp->status = HTTP_STATUS_OK;
    response_set_header(resp, "content-length", "125");
    response_set_header(resp, "content-encoding", "gzip");
    response_set_header(resp, "server", "dishfo");
    char* buf = calloc(1024 * 2, sizeof (char));
    squash_http_response(resp, buf, 1024 * 2);
    printf("%s", buf);
    printf("test response end\n");
}

void test_requset_func1() {
    int fd = open(req_text, O_RDONLY);

    if (fd < 0) {
        printf("open test file failed %s\n", strerror(errno));
        return;
    }
    int n = read(fd, filebuf, 4096);
    http_parser_execute(req_parser, &req_settings, filebuf, n);
    http_requset* req = ((custom_data*) (req_parser->data))->req;
    //  transfer(req->headers);
    printf("test request end\n");
}

void test_response_func1() {

    printf("test response end\n");
} 