#include "csapp.h"

void echo(int);

int main(int argc, char ** argv)
{
    int port, listenfd, connfd, clientlen;
    struct sockaddr_in clientaddr;
    struct hostent *hp;
    char *haddrp;

    if (argc != 2) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(0);
    }

    port = atoi(argv[1]);

    listenfd = Open_listenfd(argv[1]);
    while (1) {
	clientlen = sizeof(clientaddr);
	connfd = Accept(listenfd, (SA *) &clientaddr, &clientlen);

	/* Determine the domain name and IP address of the client */
	hp = Gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,
			    sizeof(clientaddr.sin_addr.s_addr), AF_INET);
	haddrp = inet_ntoa(clientaddr.sin_addr);
	printf("Server connected to %s:%d (%s:%d)\n", 
		hp->h_name, clientaddr.sin_port, haddrp, clientaddr.sin_port);

	echo(connfd);

	Close(connfd);
    }


    return 0;
}

void echo(int connfd)
{
    size_t n;
    char buf[MAXLINE];
    rio_t rio;

    Rio_readinitb(&rio,connfd);
    while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
	printf("Server received %u bytes\n", n);
	Rio_writen(connfd, buf, n);
    }

}


