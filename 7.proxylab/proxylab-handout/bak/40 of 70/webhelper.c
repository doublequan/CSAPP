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

