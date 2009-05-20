#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <string.h>

#include "common.h"

int main(int argc,char **argv)
{
	int listenSock, connSock, flags;
	int ret;
	struct sockaddr_in serv_addr,cli_addr;
	socklen_t server_len,client_len;
	struct sctp_sndrcvinfo sndrcvinfo;
	char buffer[MAX_BUFFER];
	client_len = sizeof(cli_addr);

	listenSock = ::socket(AF_INET,SOCK_STREAM,IPPROTO_SCTP);

	if (listenSock == -1) {
		perror("ERROR opening socket");
		exit(1);
	}
	printf("connSock : %d\n",listenSock);

	bzero(&serv_addr,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(MY_PORT_NUM);

	// bind - bind a name to a socket
	if ((::bind(listenSock,(struct sockaddr *) &serv_addr, sizeof(serv_addr)))==-1) {
		perror("listen error");
		exit(1);
	}

	if (::listen(listenSock,5)==-1) {
		perror("listen error");
		exit(1);
	}

	printf("Accepting connections ...\n");
	client_len = sizeof(cli_addr);
	connSock = ::accept(listenSock,(struct sockaddr *) &cli_addr,&client_len);
	if (connSock == -1) {
		perror("accept error");
		exit(1);
	}
	else
	{
		printf("accept the client connect...\n");
	}
	// 等待client傳送指令過來
	do {
		bzero(buffer,sizeof(buffer));
		ret = sctp_recvmsg(connSock,buffer,sizeof(buffer),(struct sockaddr *) NULL,(socklen_t *) NULL,&sndrcvinfo,&flags);
		printf("%s\n",buffer);
	}while(ret != -1 && ret != 0);
	return 0;
}
