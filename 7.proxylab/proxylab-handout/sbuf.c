#include "sbuf.h"
#include "webhelper.h"

/* Create an empty, bounded, shared FIFO buffer with n slots */
void sbuf_init(sbuf_t *sp, int n)
{
    sp->buf = web_calloc(n, sizeof(int));
    sp->n = n;
    sp->front = sp->rear = 0;
    Sem_init(&sp->mutex, 0, 1);    /* Binary semaphore for locking */
    Sem_init(&sp->slots, 0, n);    /* Initially, buf has n empty slots */
    Sem_init(&sp->items, 0, 0);    /* Initially, buf has zero data items */
}

/* Clean up buffer sp */
void sbuf_deinit(sbuf_t *sp)
{
    Free(sp->buf);
}

/* Insert item onto the rear of the shared buffer sp */
void sbuf_insert(sbuf_t *sp, int item)
{
    web_P(&sp->slots);    /* Wait for available slots                                 */
			  /* Notice that every insert will cause slots decreased by 1 */
    web_P(&sp->mutex);
    sp->buf[(++sp->rear)%(sp->n)] = item;
    web_V(&sp->mutex);

    web_V(&sp->items);    /* Annouce available item                                   */
			  /* Notice that every insert will cause items increased by 1 */
}

/* Remove and return the first item from buffer sp */
int sbuf_remove(sbuf_t *sp)
{
    int item;
    web_P(&sp->items);    /* Wait for available items                                 */
			  /* Notice that every insert will cause items decreased by 1 */
    web_P(&sp->mutex);
    item = sp->buf[(++sp->front)%(sp->n)];
    web_V(&sp->mutex);

    web_V(&sp->slots);    /* Annouce available slots                                  */
			  /* Notice that every insert will cause slots increased by 1 */
    return item;
}



