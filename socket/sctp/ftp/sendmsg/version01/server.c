//==================include header======================
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//======================================================

//===================define variable====================
#define FILEBUFFERSIZE 1024
#define HTTP_PORT 8888
#define MAXFILETRANSFER 5
#define AVAILCONNECTION 10
//======================================================

//===================define function====================
int MakePassiveSock(unsigned short int portnumber);
int RecvMsg(int,char*);
int SendMsg(int,char*);
//======================================================
int main(int argc,char *argv[])
{
	// variable
	
	char msg[] = "WELCOME\n";
	char tmp[FILEBUFFERSIZE];
	int MasterSock;
	int connfd;
	char recvbuf[FILEBUFFERSIZE];
	int ret;

	struct sockaddr_in SockAddr;
	socklen_t AddrLen = sizeof(SockAddr);
	memset(&SockAddr,0,sizeof(SockAddr));

	// 由Server建立listen，等待client連線
	MasterSock = MakePassiveSock(HTTP_PORT);
	if (MasterSock<0)
	{
		printf("MasterSock error!\n");
		exit(-1);
	}

	printf("listening....\n");
	// 一直停在這裡,等待client的連線,此同時可以等待的client連線個數就是設定的queue的大小
	connfd = accept(MasterSock,(struct sockaddr*)&SockAddr,(socklen_t*)&AddrLen);
	if (connfd < 0)
	{
		printf("accept error! return code:%d\n",connfd);
		exit(-1);
	}
	
	// 當到達這裡表示與client正確的連線了
	while(1)
	{
		// 接收字串
		if ((ret = RecvMsg(connfd,recvbuf)) == -1)
		{
			printf("Server recvive error\n");
			exit(-1);
		}
		else if (ret == 0)
		{
			close(connfd);
			printf("close connfd\n");
			close(MasterSock);
			printf("close MasterSock\n");
			return 0;
		}
		// 列印出接收的字串
		printf("cmd> %s",recvbuf);
		// 回傳字串接收成功,用來確定server有接收到資料
		SendMsg(connfd,"ok");
	}

	printf("#end#\n");
	return 0;
}

//====================function implement==================
int MakePassiveSock(unsigned short int portnumber)
{
	int PassiveSock;
	struct sockaddr_in ServAddr;
	int TransportType;
	struct sctp_initmsg initmsg;

	// setting
	memset(&ServAddr,0,sizeof(ServAddr));
	ServAddr.sin_family = PF_INET;
	ServAddr.sin_addr.s_addr = INADDR_ANY;
	ServAddr.sin_port = htons(portnumber);
	TransportType = SOCK_STREAM;

	// 1. 建立一個SCTP的socket
	PassiveSock = socket(PF_INET,TransportType,IPPROTO_SCTP);
	if (PassiveSock < 0)
	{
		printf("Error to open socket!!!\n");
		exit(-1);
	}

	// 2. 把建立的socket與address綁在一起
	if (bind(PassiveSock,(struct sockaddr*)&ServAddr,sizeof(ServAddr))<0)
	{
		printf("Error to bind socket!!!\n");
		exit(-1);
	}

	bzero(&initmsg,sizeof(initmsg));
	initmsg.sinit_num_ostreams = MAXFILETRANSFER;
	initmsg.sinit_max_instreams = MAXFILETRANSFER;
	initmsg.sinit_max_attempts = MAXFILETRANSFER-1;
	setsockopt(PassiveSock,IPPROTO_SCTP,SCTP_INITMSG,&initmsg,sizeof(initmsg));

	// 3. 設定最多通以等待幾個client端的連線,也就是可以建立的accept()
	if (listen(PassiveSock,AVAILCONNECTION)<0)
	{
		printf("Error to listen!!!\n");
		exit(-1);
	}
	// 回傳所建立的socketfd
	return PassiveSock;
}

int RecvMsg(int skt,char *msg)
{
	bzero(msg,FILEBUFFERSIZE);
	return recv(skt,msg,FILEBUFFERSIZE,0);
}

int SendMsg(int skt,char *msg)
{
	return send(skt,msg,strlen(msg),0);
}
//========================================================
