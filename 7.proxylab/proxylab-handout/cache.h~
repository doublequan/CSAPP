#ifndef __CACHE_H__
#define __CACHE_H__

#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

typedef struct node{
    char *key;
    char *cache;
    size_t size;
    struct node *prev;
    struct node *next;
} cache_n;           /* Cache Node */

typedef struct {
    cache_n head;
    size_t total_size;
    int readcnt;
    sem_t mutex;
    sem_t w;
} cache_m;            /* Cache Management Module */


void cache_init(cache_m *scheduler);
void node_init(cache_n *node, char *key, size_t size, char *cache);
cache_n *cache_read(cache_m *scheduler, char *key);
void cache_write(cache_m *scheduler, cache_n *node);
static inline void cache_remove(cache_m *scheduler, cache_n *node);
static inline void cache_insert(cache_m *scheduler, cache_n *node);



#endif
