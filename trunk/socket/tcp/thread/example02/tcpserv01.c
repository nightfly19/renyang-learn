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
int tcp_listen(const char *host,const char *serv,socklen_t *addrlenp);	// 建立listen socket
int Tcp_listen(const char *host,const char *serv,socklen_t *addrlenp);	// 建立listen socket
//==============================================================

//======================main function===========================
int main(int argc,char **argv)
{
	int listenfd,*iptr;
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
	// 若取得空間失敗,則離開程式
	if (cliaddr == NULL) {
		printf("malloc error\n");
		exit(-1);
	}
	
	for (;;) {
		len = addrlen;
		iptr = malloc(sizeof(int));	// 每一個thread自己分配一個int的空間,避免共用空間
		printf("start waiting...\n");
		*iptr = accept(listenfd,cliaddr,&len);
		if (*iptr < 0) {
			fprintf(stderr,"accept error\n");
			exit(-1);
		}
		printf("connected...\n");
		// 產生一個thread來服務client
		pthread_create(&tid,NULL,&doit,iptr);
	}
}
//==============================================================

//======================function implement======================
void str_echo(int sockfd)
{
	ssize_t n;
	char buf[MAXLINE];
	int ret_value=0;
again:
	while((n = read(sockfd,buf,MAXLINE))>0) {
		ret_value = write(sockfd,buf,n);
		if (ret_value < 0) {
			fprintf(stderr,"write socket error\n");
			pthread_exit(NULL);
		}
		printf("%s",buf);
		memset(buf,0,sizeof(buf));
	}
	if (n<0 && errno==EINTR) {
		printf("n<0 && errno==EINTER\n");
		goto again;
	}
	else
		printf("str_echo: read_error\n");
}

static void *doit(void *arg)
{
	// 如果進程中的某個線程執行了pthread_detach(th),則th線程將處於DETACHED狀態,這使得th線程在結束運行時自行釋放所佔用的內存資源,同時也無法由pthread_join()同步,pthread_detach()執行之後,對th請求pthread_join()將返回錯誤
	int connfd;
	connfd = *((int *) arg);
	free(arg);	// 釋放所指到的空間
	pthread_detach(pthread_self());
	str_echo(connfd);
	close(connfd);	// done with connected socket
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

	// 依所設定的條件(ai_flags=AI_PASSIVE;ai_family=AF_UNSPEC;ai_socktype=SOCK_STREAM)尋找本機端有用的sockaddr
	// 若hint設定為NULL，則會列出本機端的所有ip,我猜的
	// getaddrinfo=把人看的懂的名稱轉成ip
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
