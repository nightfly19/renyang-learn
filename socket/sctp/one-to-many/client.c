//========================include file==========================
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
//==============================================================

//========================define variable=======================
#define SERV_PORT 9877
//==============================================================

int main(int argc,char *argv[])
{
	int sock_fd;
	struct sockaddr_in servaddr;
	struct sctp_event_subscribe evnts;
	int echo_to_all = 0;

	if (argc < 2)
		printf("Missing host argument - use '%s host [echo]'\n",argv[0]);
	if (argc > 2) {
		printf("Echoing message to all streams\n");
		echo_to_all = 1;
	}

	sock_fd = socket(AF_INET,SOCK_SEQPACKET,IPPROTO_SCTP);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET,argv[1],&servaddr.sin_addr);

	bzero(&evnts,sizeof(evnts));
	evnts.sctp_data_io_event = 1;
	setsockopt(sock_fd,IPPROTO_SCTP,SCTP_EVENTS,&evnts,sizeof(evnts));
	// 準備傳送資料
	sctp_sendmsg(sock_fd,"good",sizeof("good"),(struct sockaddr*) &servaddr,sizeof(&servaddr),sizeof(struct sockaddr_in),22,sizeof(struct sctp_sndrcvinfo),1000,52);
	close(sock_fd);

	return 0;
}
