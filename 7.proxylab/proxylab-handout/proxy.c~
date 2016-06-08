#include <stdio.h>
#include "csapp.h"
#include "webhelper.h"
#include "cache.h"



#define WORK_THREADS_NUM (10)
/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

cache_m scheduler;

void handle_request(int);
int read_requesthdrs(rio_t *, char *);
void client_error(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg);
int parse_url(char *url, char *host, char *port, char *uri);
int str_start_with(char *string, char *t);
int header_needed(char *header);
void *work_thread(void *vargp);

int main(int argc, char **argv)
{
    int listenfd, connfd;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;

    /* Check command line args */
    if (argc != 2) {
	fprintf(stderr, "usage: %s <port>\n", argv[0]);
	exit(1);
    }

    /* Init the cache scheduler */
    cache_init(&scheduler);

    listenfd = web_open_listenfd(argv[1]);
    while (1) {
	clientlen = sizeof(clientaddr);
	/* Wait until accept someone's connect */
	connfd = web_accept(listenfd, (SA *)&clientaddr, &clientlen); 
	/* Get the client's information */
        web_getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE, 
                    port, MAXLINE, 0);
        printf("Accepted connection from (%s, %s)\n", hostname, port);

	pthread_t tid;
	web_pthread_create(&tid, NULL, work_thread, (void *)connfd);

	/* Handle the request here */
//	handle_request(connfd);

//	web_close(connfd);  
	printf("Closed connection (%s, %s)\n", hostname, port);
    }

    return 0;
}

/* Thread Function */
void *work_thread(void *vargp)
{
    int connfd = (int) vargp;
    web_pthread_detach(web_pthread_self());
    handle_request(connfd);
    web_close(connfd);
}

void handle_request(int connfd) 
{
    int serverfd, n, size;
    char request[MAXLINE], method[MAXLINE] = "", url[MAXLINE] = "", version[MAXLINE] = "";
    char headers[MAXLINE];
    rio_t rio_client, rio_server;
    char host[MAXLINE], port[MAXLINE], uri[MAXLINE];
    char buf[MAXLINE];
    char cache[MAX_OBJECT_SIZE];
    char *cachep = cache;

    /* Read request line*/
    web_rio_readinitb(&rio_client, connfd);
    if (web_rio_readlineb(&rio_client, request, MAXLINE) <= 0)
        return;
    printf("%s", request);
    sscanf(request, "%s %s %s", method, url, version);

    /* Check if the request is valid */
    if (strcmp(method, "") == 0 || strcmp(url, "") == 0 || strcmp(version, "") == 0) {
        client_error(connfd, request, "??", "Invalid",
                    "Request invalid");
	return;
    } 
    /* Method must be GET */
    if (strcasecmp(method, "GET") != 0) {                     
        client_error(connfd, method, "501", "Not Implemented",
                    "Proxy does not implement this method");
        return;
    } 

    /* Read request headers */
    if (read_requesthdrs(&rio_client, headers) == -1) {
        client_error(connfd, headers, "??", "Invalid",
                    "Headers invalid");
	return;
    }

    /* Parse the Url */
    if (parse_url(url, host, port, uri) == -1) {
        client_error(connfd, headers, "??", "Invalid",
                    "Url invalid");
	return;
    }

    /* Try to read from cache first */
    cache_n *node = cache_read(&scheduler, request);
    if (node != NULL) {   /* Found the cache */
	web_rio_writen(connfd, node->cache, node->size);
	return;
    }

    /* Forward the request to specific server */
    serverfd = web_open_clientfd(host, port);
    web_rio_readinitb(&rio_server, serverfd);
    sprintf(buf, "%s %s HTTP/1.0\r\n", method, uri);
    sprintf(buf, "%sHost: %s\r\n", buf, host);
    sprintf(buf, "%s%s", buf, user_agent_hdr);
    sprintf(buf, "%sConnection: close\r\nProxy-Connection: close\r\n", buf);
    sprintf(buf, "%s%s", buf, headers);
    web_rio_writen(serverfd, buf, strlen(buf));

    /* Read the response from server and forward to the client(brower) */
    size = 0;
    while ((n = web_rio_readnb(&rio_server, buf, MAXLINE)) > 0) {
	size += n;
	if (size <= MAX_OBJECT_SIZE) {
	    memcpy(cachep, buf, n);
	    cachep += n;
	} else {
	    cachep = NULL;
	}
	web_rio_writen(connfd, buf, n);
    }
    if (cachep != NULL) {
	node = web_malloc(sizeof(cache_n));
	node_init(node, request, size, cache);
	cache_write(&scheduler, node);
    }
}

/*
 * Parse url
 * return -1 if the url is invalid
 */
int parse_url(char *url, char *host, char *port, char *uri)
{
    char *u, *p;
    if ((url = strstr(url, "/")) == NULL)
	return -1;
    url += 2;
    u = strstr(url, "/");
    p = strstr(url, ":");
    if (u == NULL)
	return -1;
    
    if (p != NULL && strlen(p) > strlen(u)) {   /* This url includes port info */
	strncpy(host, url, strlen(url) - strlen(p));
	p++;
	strncpy(port, p, strlen(p) - strlen(u));
	strcpy(uri, u);
    } else {       /* This url does NOT include port info */
	strncpy(host, url, strlen(url) - strlen(u));
	strcpy(port, "");
	strcpy(uri, u);
    }
    return 0;
}


/*
 * read_requesthdrs - read HTTP request headers other than
 * return -1 if the headers is invalid (not end with \r\n)
 */
int read_requesthdrs(rio_t *rp, char *headers) 
{
    int n;
    char buf[MAXLINE];

    n = web_rio_readlineb(rp, buf, MAXLINE);
    if (header_needed(buf)) 
	strcat(headers, buf);
    while(n > 0 && strcmp(buf, "\r\n") != 0) {  
	n = web_rio_readlineb(rp, buf, MAXLINE);
	if (header_needed(buf)) 
	    strcat(headers, buf);
    }
    if (strcmp(buf, "\r\n") != 0) {
	web_error("headers invalid");
	return -1;
    }

    return 1;
}

/* Is the header the ones we need?
 * if yes, return 1, else return 0; 
 */
int header_needed(char *header)
{
    if (str_start_with(header, "Host:")
	|| str_start_with(header, "User-Agent:")
	|| str_start_with(header, "Connection:")
	|| str_start_with(header, "Proxy-Connection:"))
	return 0;
    else 
	return 1;
}

/* Is string start with t ? 
 * if yes, return 1, else return 0; 
 */
int str_start_with(char *string, char *t)
{
    if (strlen(string) < strlen(t)) return 0;
    while (*t != '\0') {
	if (*t != *string) return 0;
	t++;	
	string++;
    }
    return 1;
}


/*
 * client_error - returns an error message to the client
 */
void client_error(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg) 
{
    char buf[MAXLINE], body[MAXBUF];

    /* Build the HTTP response body */
    sprintf(body, "<html><title>Proxy Error</title>");
    sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
    sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
    sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
    sprintf(body, "%s<hr><em>The Web Proxy</em>\r\n", body);

    /* Print the HTTP response */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    web_rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    web_rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
    web_rio_writen(fd, buf, strlen(buf));
    web_rio_writen(fd, body, strlen(body));
}
