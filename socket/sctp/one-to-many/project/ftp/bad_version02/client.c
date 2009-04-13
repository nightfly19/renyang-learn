#include "unp.h"

int main(int argc,char **argv)
{
	int sock_fd;
	struct sockaddr_in servaddr,peeraddr;
	struct sctp_sndrcvinfo sri;
	struct sctp_event_subscribe evnts;
	int msg_flags;
	char readbuf[MAXLINE];
	socklen_t len;
	bzero(&sri,sizeof(sri));

	if (argc < 2) {
		err_quit("Missing host argument - use '%s host'\n",argv[0]);
	}
	sock_fd = Socket(AF_INET,SOCK_SEQPACKET,IPPROTO_SCTP);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	Inet_pton(AF_INET,argv[1],&servaddr.sin_addr);

	bzero(&evnts,sizeof(evnts));
	evnts.sctp_data_io_event = 1;
	Setsockopt(sock_fd,IPPROTO_SCTP,SCTP_EVENTS,&evnts,sizeof(evnts));
	
	// 接收所輸入的指令
	while((fgets(readbuf,MAXLINE,stdin)) != NULL) {
		Sctp_sendmsg(sock_fd,readbuf,strlen(readbuf),(SA *) &servaddr,sizeof(servaddr),0,0,sri.sinfo_stream,0,0);
		sri.sinfo_stream++;
		if (sri.sinfo_stream>9)
			sri.sinfo_stream = 0;
		bzero(readbuf,MAXLINE);
		len = sizeof(peeraddr);
		Sctp_recvmsg(sock_fd,readbuf,MAXLINE,(SA *) &peeraddr,&len,&sri,&msg_flags);
		printf("%s\n",readbuf);
		if (strncmp(readbuf,"open ok",strlen("open ok")))
			RecvFile(sock_fd,"copy",(SA *) &peeraddr,len,sri.sinfo_stream);
	}

	Close(sock_fd);
	return 0;
}
