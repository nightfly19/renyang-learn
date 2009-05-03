//==============================include file==========================
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
//====================================================================

//==============================define variable=======================
#define MAXLINE 80
#define SERV_PORT 8888
//====================================================================

//==============================define function=======================
void do_echo(int sockfd,struct sockaddr *pcliaddr,socklen_t clilen);
//====================================================================

//=============================main function==========================
int main(int argc,char *argv[])
{
	int sockfd;
	struct sockaddr_in servaddr,cliaddr;
	
	// create a socket, using the UDP
	sockfd = socket(AF_INET,SOCK_DGRAM,0);

	// init servaddr
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=INADDR_ANY;
	servaddr.sin_port=htons(SERV_PORT);

	// bind address and port to socket
	if (bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))==-1)
	{
		perror("bind error,the ip address is wrong");
		exit(1);
	}

	do_echo(sockfd,(struct sockaddr*)&cliaddr,sizeof(cliaddr));

	return 0;
}
//====================================================================

//==============================function implement====================
void do_echo(int sockfd,struct sockaddr *pcliaddr,socklen_t clilen)
{
	int n;
	socklen_t len;
	char mesg[MAXLINE];

	for (;;)
	{
		len=clilen;
		// wait for receive data
		// 限制一定要是由pcliaddr傳送過來的資料
		n = recvfrom(sockfd,mesg,MAXLINE,0,pcliaddr,&len);
		mesg[n]=0;
		fputs(mesg,stdout);
		// sent data back to client
		sendto(sockfd,mesg,n,0,pcliaddr,len);
	}
}
//====================================================================
