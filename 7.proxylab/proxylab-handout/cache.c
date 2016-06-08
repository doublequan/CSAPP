#include "cache.h"
#include "webhelper.h"

/* Init cache scheduler */
void cache_init(cache_m *scheduler) 
{

    scheduler->head.prev = &scheduler->head;
    scheduler->head.next = &scheduler->head;
    scheduler->total_size = 0;
    scheduler->readcnt = 0;
    web_sem_init(&scheduler->mutex, 0, 1);
    web_sem_init(&scheduler->w, 0, 1);

}

void node_init(cache_n *node, char *key, size_t size, char *cache)
{

    node->key = web_malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(node->key, key);

    node->size = size;
    node->cache = web_malloc(size);
    memcpy(node->cache, cache, size);

}

/* Try to read the cache by the given key
 * Return the cachenode if found, otherwise NULL;
 */

cache_n *cache_read(cache_m *scheduler, char *key)
{
    cache_n *rst = NULL;

    web_P(&scheduler->mutex);
    scheduler->readcnt++;
    if (scheduler->readcnt == 1) /* First in */
	web_P(&scheduler->w);
    web_V(&scheduler->mutex);

    cache_n *node = scheduler->head.next;
    while (node != &scheduler->head) {
	if (strcmp(node->key, key) == 0) {
	    rst = node;
	    break;
	}
	node = node->next;
    }
    
    /* Found the key, then put it to the very first of the list */
    if (rst != NULL) {
	rst->prev->next = rst->next;
    	rst->next->prev = rst->prev;
    	rst->next = scheduler->head.next;
    	rst->next->prev = rst;
    	rst->prev = &scheduler->head;
    	scheduler->head.next = rst;
    }

    web_P(&scheduler->mutex);
    scheduler->readcnt--;
    if (scheduler->readcnt == 0) /* Last out */
	web_V(&scheduler->w);
    web_V(&scheduler->mutex);

    return rst;
}

void cache_write(cache_m *scheduler, cache_n *node) 
{
    web_P(&scheduler->w);
    while (scheduler->total_size + node->size > MAX_CACHE_SIZE) {
	cache_remove(scheduler, scheduler->head.prev);
    }
    cache_insert(scheduler, node);
    web_V(&scheduler->w);
}

/* Remove the input cache_n
 * Dangerous function, should be protected by write mutex */
static inline void cache_remove(cache_m *scheduler, cache_n *node) 
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
    scheduler->total_size -= node->size;
    Free(node->cache);
}

/* Insert the cache_n after head
 * Dangerous function, should be protected by write mutex */
static inline void cache_insert(cache_m *scheduler, cache_n *node) 
{
    node->next = scheduler->head.next;
    node->next->prev = node;
    node->prev = &scheduler->head;
    scheduler->head.next = node;
    scheduler->total_size += node->size;
}



