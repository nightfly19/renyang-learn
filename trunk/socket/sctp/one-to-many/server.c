//==================include file======================
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//====================================================

//==================define variable===================
#define BUFFSIZE 1024
#define SERV_PORT 9877
#define LISTENQ 1024
//====================================================

int main(int argc,char *argv[])
{
	int sock_fd, msg_flags;
	char readbuf[BUFFSIZE];
	struct sockaddr_in servaddr,cliaddr;
	struct sctp_sndrcvinfo sri;
	struct sctp_event_subscribe evnets;
	int stream_increment = 1;
	socklen_t len;
	size_t rd_sz;

	if (argc == 2)
	{
		stream_increment = atoi(argv[1]);
	}

	sock_fd = socket(AF_INET,SOCK_SEQPACKET,IPPROTO_SCTP);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	if((bind(sock_fd,(struct sockaddr*) &servaddr,sizeof(servaddr)))==-1)
	{
		printf("bind error\n");
		exit(1);
	}

	bzero(&evnets,sizeof(evnets));
	evnets.sctp_data_io_event = 1;
	if (setsockopt(sock_fd,IPPROTO_SCTP,SCTP_EVENTS,&evnets,sizeof(evnets))<0) {
		printf("setsockopt error\n");
		exit(1);
	}
	
	if ((listen(sock_fd,LISTENQ))==-1)
	{
		printf("listen error\n");
		exit(1);
	}
	do{
		len = sizeof(struct sockaddr_in);
		rd_sz = sctp_recvmsg(sock_fd,readbuf,sizeof(readbuf),(struct sockaddr*) &cliaddr, &len,&sri,&msg_flags);
	}while(0);
	sctp_sendmsg(sock_fd,readbuf,rd_sz,(struct sockaddr*) &cliaddr,len,sri.sinfo_ppid,sri.sinfo_flags,
									sri.sinfo_stream,0,0);

	printf("Hello World!!\n");
	return 0;
}
