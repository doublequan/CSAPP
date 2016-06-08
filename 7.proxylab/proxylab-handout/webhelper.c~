#include "webhelper.h"

/* Handle web errors */
void web_error(char *msg)
{
    //TO DO
    printf("Web Error: %s\n", msg);
}

void web_gai_error(int code, char *msg) /* Web Getaddrinfo-style error */
{
    printf("%s: %s\n", msg, gai_strerror(code));
}

/* Web Version Robust IO functions */

void web_rio_readinitb(rio_t *rp, int fd)
{
    rio_readinitb(rp, fd);
} 

ssize_t web_rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen) 
{
    ssize_t rc;

    if ((rc = rio_readlineb(rp, usrbuf, maxlen)) < 0)
	web_error("Rio_readlineb error");
    return rc;
} 

ssize_t web_rio_readnb(rio_t *rp, void *usrbuf, size_t n) 
{
    ssize_t rc;

    if ((rc = rio_readnb(rp, usrbuf, n)) < 0)
	web_error("Rio_readnb error");
    return rc;
}

void web_rio_writen(int fd, void *usrbuf, size_t n) 
{
    if (rio_writen(fd, usrbuf, n) != n)
	web_error("Rio_writen error");
}

void web_getnameinfo(const struct sockaddr *sa, socklen_t salen, char *host, 
                 size_t hostlen, char *serv, size_t servlen, int flags)
{
    int rc;

    if ((rc = getnameinfo(sa, salen, host, hostlen, serv, 
                          servlen, flags)) != 0) 
        web_gai_error(rc, "Getnameinfo error");
}

int web_accept(int s, struct sockaddr *addr, socklen_t *addrlen) 
{
    int rc;

    if ((rc = accept(s, addr, addrlen)) < 0)
	web_error("Accept error");
    return rc;
}

void web_close(int fd) 
{
    int rc;

    if ((rc = close(fd)) < 0)
	web_error("Close error");
}

int web_open_clientfd(char *hostname, char *port) 
{
    int rc;

    if ((rc = open_clientfd(hostname, port)) < 0) 
	web_error("Open_clientfd error");
    return rc;
}

int web_open_listenfd(char *port) 
{
    int rc;

    if ((rc = open_listenfd(port)) < 0)
	web_error("Open_listenfd error");
    return rc;
}

void *web_calloc(size_t nmemb, size_t size) 
{
    void *p;

    if ((p = calloc(nmemb, size)) == NULL)
	web_error("Calloc error");
    return p;
}

void *web_malloc(size_t size) 
{
    void *p;

    if ((p  = malloc(size)) == NULL)
	web_error("Malloc error");
    return p;
}

/*******************************
 * Wrappers for Posix semaphores
 *******************************/

void web_sem_init(sem_t *sem, int pshared, unsigned int value) 
{
    if (sem_init(sem, pshared, value) < 0)
	web_error("Sem_init error");
}

void web_P(sem_t *sem) 
{
    if (sem_wait(sem) < 0)
	web_error("P error");
}

void web_V(sem_t *sem) 
{
    if (sem_post(sem) < 0)
	web_error("V error");
}

/************************************************
 * Wrappers for Pthreads thread control functions
 ************************************************/

void web_pthread_create(pthread_t *tidp, pthread_attr_t *attrp, 
		    void * (*routine)(void *), void *argp) 
{
    int rc;

    if ((rc = pthread_create(tidp, attrp, routine, argp)) != 0)
	web_error("Pthread_create error");
}

void web_pthread_cancel(pthread_t tid) 
{
    int rc;

    if ((rc = pthread_cancel(tid)) != 0)
	web_error("Pthread_cancel error");
}

void web_pthread_join(pthread_t tid, void **thread_return) 
{
    int rc;

    if ((rc = pthread_join(tid, thread_return)) != 0)
	web_error("Pthread_join error");
}

/* $begin detach */
void web_pthread_detach(pthread_t tid) 
{
    int rc;

    if ((rc = pthread_detach(tid)) != 0)
	web_error("Pthread_detach error");
}
/* $end detach */

void web_pthread_exit(void *retval) 
{
    pthread_exit(retval);
}

pthread_t web_pthread_self(void) 
{
    return pthread_self();
}
 
void web_pthread_once(pthread_once_t *once_control, void (*init_function)()) 
{
    pthread_once(once_control, init_function);
}




