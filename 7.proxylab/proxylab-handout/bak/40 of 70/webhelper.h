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
