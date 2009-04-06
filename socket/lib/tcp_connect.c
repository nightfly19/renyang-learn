#include "unp.h"

int tcp_connect(const char *host,const char *serv)
{
	int sockfd,n;
	struct addrinfo hints,*res,*ressave;

	bzero(&hints,sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((n=getaddrinfo(host,serv,&hints,&res))!=0)
		err_quit("tcp_connect error for %s, %s: %s",host,serv,gai_strerror(n));
	// res內儲存許多要建立的socket參數
	ressave = res;

	do {
		// 嘗試建立socket
		sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
		if (sockfd < 0)		// 建立失敗,尋找下一個要建立socket的參數
			continue;	// ignore this one
		if (connect(sockfd,res->ai_addr,res->ai_addrlen) == 0)
			break;		// success

		Close(sockfd);
	} while ((res=res->ai_next)!=NULL);

	if (res == NULL)	// 利用了所以可以建立socket的參數,但均是失敗,則列印出錯誤
		err_sys("tcp_connect error for %s, %s",host, serv);

	// 把剛剛收集到的可以嘗試建立socket的參數陣列的空間釋放出來
	freeaddrinfo(ressave);

	return sockfd;
}

int Tcp_connect(const char *host,const char *serv)
{
	return (tcp_connect(host,serv));
}
