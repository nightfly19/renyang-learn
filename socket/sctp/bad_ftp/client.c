//=========================include header========================
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//================================================================

//==========================define variable=======================
#define HTTP_PORT 8888
#define FILEBUFFERSIZE 1024
#define MAXFILETRANSFER 5
//================================================================

//================================================================
int ServerSock;
//================================================================

//===========================define function======================
int ConnectToServer(char *);
int RecvMsg(int,char *);
int SendMsg(int,char *);
//================================================================
int main(int argc,char *argv[])
{
	// variable
	char ipaddr[]="127.0.0.1";
	char sendbuffer[FILEBUFFERSIZE];
	char recvbuffer[FILEBUFFERSIZE];
	struct sockaddr_in SockAddr;
	int AddrLen = sizeof(SockAddr);

	// initialize
	// memset(ipaddr,0,sizeof(ipaddr));

	// 與server端連線
	ServerSock = ConnectToServer(ipaddr);
	printf("waiting for welcome....\n");
	while(1) {
		// 由使用者輸入字串
		printf("Please input string:\n");
		fgets(sendbuffer,FILEBUFFERSIZE,stdin);
		if (strncmp(sendbuffer,"exit\n",strlen("exit\n"))==0)
		{
			break;
		}
		SendMsg(ServerSock,sendbuffer);
		memset(recvbuffer,0,FILEBUFFERSIZE);
		RecvMsg(ServerSock,recvbuffer);
		if (strncmp(recvbuffer,"ok",strlen("ok")))
		{
			break;
		}
	}

	printf("#end#\n");
	return 0;
}

//==================function implement===============================
int ConnectToServer(char *ipaddr)
{
	int ServerSock;
	struct sockaddr_in ServAddr;
	int TransportType;
	int ret;
	struct sctp_event_subscribe events;
	struct sctp_initmsg initmsg;

	// setting
	memset(&ServAddr,0,sizeof(ServAddr));
	ServAddr.sin_family = PF_INET;
	ServAddr.sin_addr.s_addr = inet_addr(ipaddr);
	ServAddr.sin_port = htons(HTTP_PORT);
	TransportType = SOCK_STREAM;	// 以stream為導向

	// 1. 打開一個socket
	ServerSock = socket(PF_INET,TransportType,IPPROTO_SCTP);
	if (ServerSock < 0)
	{
		printf("Error to open socket!!!\n");
		exit(-1);
	}

	bzero(&initmsg,sizeof(initmsg));
	initmsg.sinit_num_ostreams = MAXFILETRANSFER;
	initmsg.sinit_max_instreams = MAXFILETRANSFER;
	initmsg.sinit_max_attempts = MAXFILETRANSFER - 1;
	setsockopt(ServerSock,IPPROTO_SCTP,SCTP_INITMSG,&initmsg,sizeof(initmsg));
	
	// 連到server
	if (connect(ServerSock,(struct sockaddr *)&ServAddr,sizeof(ServAddr)) < 0)
	{
		printf("Error to Connect socket!!!\n");
		exit(-1);
	}

	// Enable receipt of SCTP Snd/Rcv Data via sctp_recvmsg
	bzero(&events,sizeof(events));
	events.sctp_data_io_event = 1;
	ret = setsockopt(ServerSock,IPPROTO_SCTP,SCTP_EVENTS,(const void*)&events,sizeof(events));
	printf("ret:%d\n",ret);
	return ServerSock;
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
//===================================================================
