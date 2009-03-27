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
#include <pthread.h>
//==============================================================

//======================define variable=========================
#include "global.h"
//==============================================================

//======================global variable=========================
static int sockfd;
static FILE *fp;
//==============================================================

//======================define function=========================
void str_cli(FILE *fp,int sockfd);
void *copyto(void *);
//==============================================================

//======================main function===========================
int main(int argc,char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;
	int ret_value=0;

	if (argc != 2) {
		printf("usage: tcpcli <IPaddress>\n");
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
void str_cli(FILE *fp_arg,int sockfd_arg)
{
	char recvline[MAXLINE];
	int ret_value;
	pthread_t tid;
	sockfd = sockfd_arg;	// copy arguments to externals
	fp = fp_arg;
	pthread_create(&tid,NULL,copyto,NULL);
	if (ret_value !=0) {
		printf("pthread_create error\n");
		exit(-1);
	}
	while (read(sockfd,recvline,MAXLINE)>0) {
		fputs(recvline,stdout);
		memset(recvline,0,sizeof(recvline));
	}
}

void *copyto(void *arg)
{
	char sendline[MAXLINE];
	while (fgets(sendline,MAXLINE,fp)!=NULL)
		write(sockfd,sendline,strlen(sendline));
	shutdown(sockfd,SHUT_WR);
	return (NULL);
}
