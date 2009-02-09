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
//================================================================
int main(int argc,char *argv[])
{
	// variable
	char ipaddr[15]="127.0.0.1";
	char recvbuffer[FILEBUFFERSIZE];
	char sendbuffer[FILEBUFFERSIZE];
	char filebuffer[FILEBUFFERSIZE];
	char *filelist;
	struct sockaddr_in SockAddr;
	int AddrLen = sizeof(SockAddr);

	// initialize
	memset(ipaddr,0,sizeof(ipaddr));

	ServerSock = ConnectToServer(ipaddr);

	printf("#end#");
	return 0;
}

//==================function implement===============================
int ConnectToServer(char *ipaddr)
{
	return 0;
}
//===================================================================
