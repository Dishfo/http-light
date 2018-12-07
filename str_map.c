/*      
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "string_map.h"

static void free_bucket(map_bucket *bucket);
static void free_node(map_node *node);
static unsigned long int hash(const char* str);
static void* add_node(map_bucket* bkt,
        const char *key, const void *val, size_t len, int set);
static map_node* find_node(map_bucket* bkt, const char *key);
static unsigned long int hash_code(const char* str);
static map_node* remove_ndoe(map_bucket* bkt, const char*key);

void map_init(str_map* map) {
    printf("init bkt %p  \n",map);
    if (map == NULL) {
        return;
    }

    if (map->table != NULL) {
        return;
    }
  //  printf("hehehehehe %d\n",16);
    map->table = malloc(INIT_CAP * sizeof (map_bucket));
    map->cap = INIT_CAP;
    map->size = 0;
   // printf("hehehehehe %d\n",16);
    for (int i = 0; i < INIT_CAP; i++) {
        map_node *n = calloc(1, sizeof (map_node));
      //  printf("init head %p\n",n);
        map->table[i].head = n;
        (map->table)[i].size = 0;
    }
}

const void* map_get(str_map *map, const void *key, size_t* len) {
    map_bucket* table;
    map_bucket bkt;
    if (map == NULL ||
            key == NULL) {
        return NULL;
    }
    if (map->table == NULL) {
        return NULL;
    }
    table = map->table;
    int hash = hash_code(key);
    bkt = table[hash & (map->cap - 1)];
    map_node *res = find_node(&bkt, key);
    if (res == NULL) {
        SET_VLEN(len, 0)
        return NULL;
    } else {
        SET_VLEN(len, res->val_len);
        return res->val;
    }
}

void* map_put(str_map *map, const char *key, const void *val, size_t len, int set) {
    if (map == NULL ||
            key == NULL ||
            val == NULL) {
        return NULL;
    }

    if (map->table == NULL) {
        return NULL;
    }
    map_bucket* table;
    map_bucket bkt;
    table = map->table;
    int hash = hash_code(key);
    bkt = table[hash & (map->cap - 1)];
    void* res = add_node(&bkt, key, val, len, set);

    if (res == NULL) {

        map->size++;
    }
    return res;
}

void * map_remove(str_map *map, const char *key, size_t* len) {
    map_bucket* table;
    map_bucket bkt;
    char *res = NULL;
    if (map == NULL ||
            key == NULL) {
        return NULL;
    }
    if (map->table == NULL) {
        return NULL;
    }
    table = map->table;
    int hash = hash_code(key);
    bkt = table[hash & (map->cap - 1)];
    map_node* rmnode = remove_ndoe(&bkt, key);
    if (rmnode != NULL) {
        SET_VLEN(len, rmnode->val_len)
        map->size--;
        res = rmnode->val;
        rmnode->val = NULL;
        FREE_MAPNODE(rmnode);
        return res;
    } else {
        SET_VLEN(len, 0);

        return NULL;
    }
}

void free_map(str_map* map) {
    if (map == NULL || map->table == NULL) {
        return;
    }
    for (int i = 0; i < map->size; i++) {
        map_bucket* bkt = &map->table[i];
        map_node* cur = bkt->head;
        map_node* tmp = NULL;
        while (cur != NULL) {

            tmp = cur;
            cur = cur->next;
            FREE_MAPNODE(tmp);
        }
        bkt->head = NULL;
    }
}

static unsigned long int hash_code(const char* str) {
    int hash = 0;
    for (int i = 0; str[i] != '\0'; i++) {

        hash += (str[i]);
    }
    return hash;
}

static void* add_node(map_bucket* bkt,
        const char *key, const void *val, size_t len, int set) {

    if (bkt == NULL || key == NULL || val == NULL) {
        return NULL;
    }

    map_node* pre = bkt->head;
    //printf("head is  %p bkt is %p\n", pre, bkt);
    map_node* cur = bkt->head->next;
    while (cur != NULL) {
        if (strcmp(cur->name, key) == 0) {
            break;
        }
        pre = cur;
        cur = cur->next;
    }

    if (cur == NULL) {
        map_node *newn = malloc(sizeof (map_node));
        newn->val = malloc(len);
        int nl = strlen(key);
        newn->name = malloc(nl + 1);
        strncpy(newn->name, key, nl);
        newn->name[nl] = 0;
        newn->val_len = len;
        memcpy(newn->val, val, len);
        pre->next = newn;
        newn->next = NULL;
    } else {
        char* res = cur->val;
        if (set == SET_NEWVAL) {
            void* old = cur->val;
            cur->val_len = len;
            cur->val = malloc(len);
            memcpy(cur->val, val, len);

            return old;
        }
    }

    return NULL;
}

static map_node* remove_ndoe(map_bucket* bkt, const char*key) {

    if (bkt == NULL) {
        return NULL;
    }

    map_node* cur = bkt->head->next;
    map_node* pre = bkt->head;

    while (cur != NULL) {
        if (strcmp(key, cur->name) == 0) {
            break;
        }
        pre = cur;
        cur = cur->next;
    }
    if (cur != NULL) {

        pre->next = cur->next;
        bkt->size--;
    }
    return cur;
}

static map_node* find_node(map_bucket* bkt, const char *key) {
    if (bkt == NULL || key == NULL) {
        return NULL;
    }

    map_node* pre = bkt->head;
    map_node* cur = bkt->head->next;
    while (cur != NULL) {
        if (strcmp(cur->name, key) == 0) {
            break;
        }
        pre = cur;
        cur = cur->next;
    }

    return cur;
}





