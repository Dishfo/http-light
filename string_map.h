/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   string_map.h
 * Author: dishfo
 *
 * Created on 2018年12月2日, 下午8:07
 */

#ifndef STRING_MAP_H
#define STRING_MAP_H

#include <stdio.h>


#define INIT_CAP 16
#define SET_NEWVAL 1
#define SET_ART 2

#define FREE_MAPNODE(node) \
free(node->name); \
free(node->val); \
free(node)

#define SET_VLEN(l_ptr,len) \
if((l_ptr)!=NULL){ \
  *(l_ptr)=(len) ;\
}

typedef struct str_map str_map;
typedef struct map_node map_node;
typedef struct map_bucket map_bucket;

#ifdef __cplusplus
extern "C" {
#endif    
#ifdef __cplusplus
}
#endif

struct str_map {
    int cap;
    int size;
    map_bucket* table;
};
//val_len > 0 mean specfic or -1 is not know 0 is not available
struct map_node {
    char* name;
    void* val;
    size_t val_len;
    map_node* next;
};

struct map_bucket {
    int size;
    map_node* head;
};

extern void* map_put(str_map *map, const char *key,const void *val,size_t len,int set);
extern const void* map_get(str_map *map, const void *key,size_t* len);
extern void* map_remove(str_map *map, const char *key,size_t* len);
extern void map_init(str_map* map);
extern void free_map(str_map* map);
            
#endif /* STRING_MAP_H */






