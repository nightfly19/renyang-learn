#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/sctp.h>
#include <string.h>
#include <stdio.h>

int main(int argc,char **argv)
{
	int sock_fd,msg_flags;
	char readbuf[1024];
	struct sockaddr_in servaddr,cliaddr;
	struct sctp_sndrcvinfo sri;
	struct sctp_event_subscribe evnts;
	socklen_t len;
	size_t rd_sz;

	if ((sock_fd = socket(AF_INET,SOCK_SEQPACKET,IPPROTO_SCTP)) < 0) {
		perror("socket");
	}

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(9877);
	if (bind(sock_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind");
	}

	bzero(&evnts, sizeof(evnts));
	evnts.sctp_data_io_event = 1;
	if (setsockopt(sock_fd,IPPROTO_SCTP, SCTP_EVENTS, &evnts, sizeof(evnts)) < 0) {
		perror("setsockopt");
	}

	if (listen(sock_fd,5) < 0) {
		perror("listen");
	}

	for (;;) {
		len = sizeof(struct sockaddr_in);
		printf("start wait...\n");
		rd_sz = sctp_recvmsg(sock_fd,readbuf, sizeof(readbuf), (struct sockaddr *)&cliaddr, &len, &sri, &msg_flags);
		printf("sctp_recvmsg() received %d bytes from %s:%d.\n", (int)rd_sz, inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
		if (rd_sz > 0) {
			if (sctp_sendmsg(sock_fd, readbuf, rd_sz, (struct sockaddr *)&cliaddr, len, sri.sinfo_ppid, sri.sinfo_flags, sri.sinfo_stream, 0, 0) < 0) {
				perror("sctp_sendmsg");
			}
		}
	}
	return 0;
}
