#ifndef __WEBHELPER_H__
#define __WEBHELPER_H__

#include "csapp.h"

void web_rio_readinitb(rio_t *rp, int fd);
ssize_t web_rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);
void web_rio_writen(int fd, void *usrbuf, size_t n); 
void web_error(char *msg);
void web_getnameinfo(const struct sockaddr *sa, socklen_t salen, char *host, 
                 size_t hostlen, char *serv, size_t servlen, int flags);
int web_accept(int s, struct sockaddr *addr, socklen_t *addrlen);
void web_close(int fd);
int web_open_clientfd(char *hostname, char *port);
int web_open_listenfd(char *port);
ssize_t web_rio_readnb(rio_t *rp, void *usrbuf, size_t n);
void *web_calloc(size_t nmemb, size_t size);
void web_sem_init(sem_t *sem, int pshared, unsigned int value);
void web_P(sem_t *sem);
void web_V(sem_t *sem);
void web_pthread_create(pthread_t *tidp, pthread_attr_t *attrp, 
		    void * (*routine)(void *), void *argp);
void web_pthread_cancel(pthread_t tid);
void web_pthread_join(pthread_t tid, void **thread_return);
void web_pthread_detach(pthread_t tid);
void web_pthread_exit(void *retval);
pthread_t web_pthread_self(void);
void web_pthread_once(pthread_once_t *once_control, void (*init_function)());
void *web_malloc(size_t size);


#endif
