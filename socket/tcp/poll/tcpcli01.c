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
	char sendline[MAXLINE],recvline[MAXLINE];
	int ret_value=0;

	FD_ZERO(&rset);	// 清空
	for (;;) {
		FD_SET(fileno(fp),&rset);	// 設定要監控的file descriptor
		FD_SET(sockfd,&rset);
		maxfdp1 = max(fileno(fp),sockfd)+1;
		select(maxfdp1,&rset,NULL,NULL,NULL);

		if (FD_ISSET(sockfd,&rset)) {	// socket is readable
			printf("socket is readable\n");
			memset(recvline,0,sizeof(recvline));
			if (read(sockfd,recvline,MAXLINE)==0) {
				printf("str_cli:server terminated prematurely");
			}
			fputs(recvline,stdout);
		}

		if (FD_ISSET(fileno(fp),&rset)) {	// input is readable
			printf("input is readable\n");
			if (fgets(sendline,MAXLINE,fp) == NULL) {
				printf("sendline is null\n");
				return ;
			}
			ret_value = write(sockfd,sendline,strlen(sendline));
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
