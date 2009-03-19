//======================include file============================
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
//==============================================================

//======================define variable=========================
#include "global.h"
//==============================================================

//======================define function=========================
void str_cli(FILE *fp,int sockfd);
int max(int a,int b);
//==============================================================

//======================main function===========================
int main(int argc,char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;
	int ret_value=0;

	if (argc != 2) {
		printf("usage: tcpcli <IPaddress>");
		exit(-1);
	}

	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if (sockfd == -1) {
		printf("socket error\n");
		exit(-1);
	}

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	ret_value = inet_pton(AF_INET,argv[1],&servaddr.sin_addr);
	if (ret_value == -1) {
		printf("inet_pton error\n");
		exit(-1);
	}

	ret_value = connect(sockfd,(struct sockaddr *) &servaddr,sizeof(servaddr));
	if (ret_value == -1) {
		perror("connect error\n");
		exit(-1);
	}

	str_cli(stdin,sockfd);

	exit(0);
}
//==============================================================

//======================function implement======================
void str_cli(FILE *fp,int sockfd)
{
	int maxfdp1;
	fd_set rset;
	char buf[MAXLINE];
	int n;
	int ret_value=0;

	int stdineof=0;

	FD_ZERO(&rset);	// 清空
	for (;;) {
		if (stdineof==0) {
			FD_SET(fileno(fp),&rset);	// 設定要監控的file descriptor
		}
		FD_SET(sockfd,&rset);
		maxfdp1 = max(fileno(fp),sockfd)+1;
		select(maxfdp1,&rset,NULL,NULL,NULL);

		if (FD_ISSET(sockfd,&rset)) {	// socket is readable
			printf("socket is readable\n");
			if ((n=read(sockfd,buf,MAXLINE))==0) {
				if (stdineof==1)
					return ; // normal termination
				else
				{
					printf("str_cli:server terminated prematurely");
					exit(-1);
				}
			}
			write(fileno(stdout),buf,n);	// 把收集到的資料印出來
		}

		if (FD_ISSET(fileno(fp),&rset)) {	// input is readable
			printf("input is readable\n");
			if ((n=read(fileno(fp),buf,MAXLINE))==0) {
				stdineof = 1;
				shutdown(sockfd,SHUT_WR); // send FIN
				FD_CLR(fileno(fp),&rset);
				continue;
			}
			ret_value = write(sockfd,buf,n);
			if (ret_value == -1) {
				printf("write error\n");
				exit(-1);
			}
		}
	}
}

int max(int a,int b)
{
	return (a>b)?a:b;
}
