//=============================include file================================
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
//=========================================================================

//=============================define variable=============================
#define MAXLINE 80
#define SERV_PORT 8888
//=========================================================================

//=============================define function=============================
void do_cli(FILE *fp,int sockfd,struct sockaddr *pservaddr,socklen_t servlen);
//=========================================================================

//=============================main function===============================
int main(int argc,char *argv[])
{
	int sockfd;
	struct sockaddr_in servaddr;

	// check args
	if (argc!=2)
	{
		perror("usage:udpclient <IP address>\n");
		exit(1);
	}

	// init servaddr
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(SERV_PORT);
	if(inet_pton(AF_INET,argv[1],&servaddr.sin_addr)<=0)
	{
		printf("[%s] is not a valid IP address\n",argv[1]);
		exit(1);
	}

	// build the socket
	sockfd=socket(AF_INET,SOCK_DGRAM,0);

	do_cli(stdin,sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));

	return 0;
}
//=========================================================================


//=============================function implement==========================
void do_cli(FILE *fp,int sockfd,struct sockaddr *pservaddr,socklen_t servlen)
{
	int n;
	char sendline[MAXLINE],recvline[MAXLINE+1];

	// connect to server
	if (connect(sockfd,(struct sockaddr *)pservaddr,servlen)==-1)
	{
		perror("connect error");
		exit(1);
	}

	while (fgets(sendline,MAXLINE,fp)!=NULL)
	{
		// read a line and send to server
		write(sockfd,sendline,strlen(sendline));
		// receive data from server
		n=read(sockfd,recvline,MAXLINE);
		if (n==-1)
		{
			perror("read error");
			exit(1);
		}
		// terminate string
		// 依字串的最後面加上一個結束符號
		recvline[n]=0;
		fputs(recvline,stdout);
	}
}
//=========================================================================
