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
//======================================================
int main(int argc,char *argv[])
{
	// variable
	
	char msg[] = "WELCOME\n";
	char tmp[FILEBUFFERSIZE];
	int MasterSock;
	int connfd;

	struct sockaddr_in SockAddr;
	socklen_t AddrLen = sizeof(SockAddr);
	memset(&SockAddr,0,sizeof(SockAddr));

	MasterSock = MakePassiveSock(HTTP_PORT);
	if (MasterSock<0)
	{
		printf("MasterSock error!\n");
		exit(-1);
	}

	while(1)
	{
		printf("listening....\n");
		connfd = accept(MasterSock,(struct sockaddr*)&SockAddr,(socklen_t*)&AddrLen);
		if (connfd < 0)
		{
			printf("accept error! return code:%d\n",connfd);
			exit(-1);
		}
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

	// open a socket
	PassiveSock = socket(PF_INET,TransportType,IPPROTO_SCTP);
	if (PassiveSock < 0)
	{
		printf("Error to open socket!!!\n");
		exit(-1);
	}

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

	if (listen(PassiveSock,AVAILCONNECTION)<0)
	{
		printf("Error to listen!!!\n");
		exit(-1);
	}
	return PassiveSock;
}
//========================================================
