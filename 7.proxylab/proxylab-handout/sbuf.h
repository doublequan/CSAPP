/*
 * sbuf.h - for shared buf
 */
#ifndef __SBUF_H__
#define __SBUF_H__

#include "csapp.h"

typedef struct {
    int *buf;      /* Buffer array */
    int n;         /* Max num of slots */
    int front;     /* buf[(front+1)%n] is the first item */
    int rear;      /* buf[rear%n] is the last item */
    sem_t mutex;   /* Protects accesses to buf */
    sem_t slots;   /* Counts available alots */
    sem_t items;   /* Counts available items */
} sbuf_t;



#endif /* __SBUF_H__ */
