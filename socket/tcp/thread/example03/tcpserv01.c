//======================include file============================
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <netdb.h>
//==============================================================

//======================define variable=========================
#include "global.h"
//==============================================================

//======================define function=========================
static void *doit(void *);
void str_echo(int sockfd);
int tcp_listen(const char *host,const char *serv,socklen_t *addrlenp);
int Tcp_listen(const char *host,const char *serv,socklen_t *addrlenp);
//==============================================================

//======================main function===========================
int main(int argc,char **argv)
{
	int listenfd,connfd;
	pthread_t tid;
	socklen_t addrlen,len;
	struct sockaddr *cliaddr;

	if (argc == 2)
		listenfd = Tcp_listen(NULL,argv[1],&addrlen);
	else if (argc == 3)
		listenfd = Tcp_listen(argv[1],argv[2],&addrlen);
	else
	{
		printf("usage: tcpserv01 [ <host> ] <service or port>\n");
		exit(-1);
	}
	cliaddr = malloc(addrlen);
	if (cliaddr == NULL) {
		printf("malloc error\n");
		exit(-1);
	}
	
	for (;;) {
		len = addrlen;
		printf("start waiting...\n");
		connfd = accept(listenfd,cliaddr,&len);
		if (connfd < 0) {
			fprintf(stderr,"accept error\n");
			exit(-1);
		}
		printf("connected...\n");
		// 產生一個thread來服務client
		pthread_create(&tid,NULL,&doit,(void *) connfd);
	}
}
//==============================================================

//======================function implement======================
void str_echo(int sockfd)
{
	ssize_t n;
	char buf[MAXLINE];
	int ret_value=0;
	int value=0x5001;
	int return_value1=0x1010;
	int return_value2=0x0101;
again:
	// renyang - read data from client
	while((n = read(sockfd,buf,MAXLINE))>0) {

		// renyang - compare the receiving data and send the corresponding response - start
		if (memcmp(buf,&value,4)==0)
		{
			// renyang - sending 4 bytes that is 0x1010
			ret_value = write(sockfd,&return_value1,4);
		}
		else
		{
			// renyang - sending 4 bytes that is 0x0101
			ret_value = write(sockfd,&return_value2,4);
		}
		// renyang - compare the receiving data and send the corresponding response - end

		if (ret_value < 0) {
			fprintf(stderr,"write socket error\n");
			pthread_exit(NULL);
		}
		memset(buf,0,sizeof(buf));
	}
	if (n<0 && errno==EINTR) {
		printf("n<0 && errno==EINTER\n");
		goto again;
	}
	if (n==0)
		printf("one of connections is disconnect\n");
	else
		printf("str_echo: read_error\n");
}

static void *doit(void *arg)
{
	pthread_detach(pthread_self());
	str_echo((int) arg);
	close((int) arg);
	return (NULL);
}

int Tcp_listen(const char *host,const char *serv,socklen_t *addrlenp)
{
	return (tcp_listen(host,serv,addrlenp));
}

int tcp_listen(const char *host,const char *serv,socklen_t *addrlenp)
{
	int listenfd,n;
	const int on = 1;
	struct addrinfo hints,*res,*ressave;

	bzero(&hints,sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((n=getaddrinfo(host,serv,&hints,&res))!=0) {
		printf("tcp_listen error for %s, %s: %s\n",host,serv,strerror(n));
		pthread_exit(NULL);
	}
	ressave = res;
	do {
		listenfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
		if (listenfd < 0)	// 建立listen socket失敗,尋找下一個
			continue;
		setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
		if (bind(listenfd,res->ai_addr,res->ai_addrlen)==0)
			break;
		close(listenfd);
	}while((res=res->ai_next)!=NULL);

	if (res == NULL) {
		printf("tcp_listen error for %s, %s\n",host,serv);
		pthread_exit(NULL);
	}
	listen(listenfd,LISTENQ);
	if (addrlenp)
		*addrlenp = res->ai_addrlen;
	freeaddrinfo(ressave);
	return (listenfd);
}
