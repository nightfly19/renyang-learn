#include "unp.h"

int main(int argc,char **argv)
{
	int sock_fd,msg_flags;
	char readbuf[BUFFSIZE];
	char filename[BUFFSIZE];
	struct sockaddr_in servaddr, cliaddr;
	struct sctp_sndrcvinfo sri;
	struct sctp_event_subscribe evnts;
	int stream_increment = 1;	// 每一次stream no增加1
	socklen_t len;
	size_t rd_sz;

	sock_fd = Socket(AF_INET,SOCK_SEQPACKET,IPPROTO_SCTP);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	Bind(sock_fd,(SA *) &servaddr,sizeof(servaddr));

	// 把所有的event事件均打開
	SctpTurnOffAllEvent(sock_fd);

	Listen(sock_fd,LISTENQ);
	printf("Start waiting...\n");
	for (;;) {
		len = sizeof(struct sockaddr_in);
		rd_sz = Sctp_recvmsg(sock_fd,readbuf,sizeof(readbuf),(SA *) &cliaddr,&len,&sri,&msg_flags);
		if (msg_flags & MSG_NOTIFICATION) {
			print_notification(readbuf);
			continue;
		}
		printf("You input is %s",readbuf);
		if (strncmp(readbuf,"#getfile ",strlen("#getfile "))==0) {
			printf("You want to get file\n");
			bzero(filename,BUFFSIZE);
			strcpy(filename,readbuf + strlen("#getfile "));
			printf("%s",filename);
		}
		bzero(&readbuf,sizeof(readbuf));
	}
	return 0;
}
