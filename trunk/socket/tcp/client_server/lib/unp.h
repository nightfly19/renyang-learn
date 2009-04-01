#ifndef UNP_H
#define UNP_H

//==============================include header==========================
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
//======================================================================

//==============================define variable=========================
#define MAXN 16384      // max # bytes to request from server
#define MAXLINE 4096
//======================================================================

//==============================define function=========================
int tcp_connect(const char *host,const char *serv);
int Tcp_connect(const char *host,const char *serv);
ssize_t writen(int fd,const void *vptr,size_t n);
void Writen(int fd,void *ptr,size_t nbytes);
void err_sys(const char *fmt);
//======================================================================

//==============================function implement======================
int tcp_connect(const char *host,const char *serv)
{
        int sockfd,n;
        struct addrinfo hints,*res,*ressave;
        bzero(&hints,sizeof(struct addrinfo));
        // 設定依條件所建立的socket
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        if ((n=getaddrinfo(host,serv,&hints,&res))!=0){
                fprintf(stderr,"tcp_connect error for %s,%s:%s",host,serv,gai_strerror(n));
                exit(-1);
        }
        // res內儲存許多要建立的socket參數
        ressave = res;
        do {
                // 嘗試建立socket
                sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
                if (sockfd < 0)         // 建立失敗,尋找下一個要建立socket的參數
                        continue;       // ignore this one
                if (connect(sockfd,res->ai_addr,res->ai_addrlen)==0)
                        break;          // success
                close(sockfd);          // ignore this one
        } while ((res=res->ai_next)!=NULL);

        if (res ==NULL) {       // 利用了所以可以建立socket的參數,但均是失敗,則列印出錯誤
                fprintf(stderr,"tcp_connect error for %s,%s",host,serv);
                exit(-1);
        }
        // 把剛剛收集到的可以嘗試建立socket的參數陣列的空間釋放出來
        freeaddrinfo(ressave);
        return (sockfd);
}

int Tcp_connect(const char *host,const char *serv)
{
	return (tcp_connect(host,serv));
}

ssize_t writen(int fd,const void *vptr,size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = vptr;
	nleft = n;
	while (nleft>0) {
		if ((nwritten = write(fd,ptr,nleft)) <=0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;	// and call write() again
			else
				return (-1);
		}
		nleft -= nwritten;
		ptr += nwritten;
	}
	return (n);
}
// end writen

void Writen(int fd,void *ptr,size_t nbytes)
{
	if (writen(fd,ptr,nbytes) != nbytes)
		err_sys("writen error");
}

void err_sys(const char *fmt) {
	fprintf(stderr,"%s\n",fmt);
	exit(1);
}
//======================================================================

#endif
