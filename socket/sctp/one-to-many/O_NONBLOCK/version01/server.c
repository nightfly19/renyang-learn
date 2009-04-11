//=========================include header===============================
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
//======================================================================

//=========================main function================================
int main(int argc, char **argv)
{
	int sock_fd,msg_flags;
	char readbuf[200];
	struct sockaddr_in cliaddr;
	struct sctp_sndrcvinfo sri;
	struct sctp_event_subscribe evnts;
	socklen_t len;
	size_t rd_sz;
	struct sockaddr_in *paddr, a;
	int port = 2905;
	unsigned char flag = 1;
	paddr = &a;
	paddr->sin_family = AF_INET;
	paddr->sin_port = htons(port);
	paddr->sin_addr.s_addr = INADDR_ANY;
	sock_fd = socket(AF_INET6, SOCK_SEQPACKET, IPPROTO_SCTP);

	if(fcntl(sock_fd,F_SETFL,O_NONBLOCK) <0)
	{
		printf("\r\n PID_UP::fcntl:O_NONBLOCK error\n ");
		return -1;
	}

	if (-1 == bind(sock_fd, (struct sockaddr*)paddr, sizeof(struct sockaddr))) {
		printf( "ERRNO:%d\n ", errno);
		perror( "E_bind_Fail ");
		return -1;
	}
	bzero(&evnts, sizeof(evnts)); 
	evnts.sctp_data_io_event = 1; 
	/* end mod_serv07 */ 
	evnts.sctp_association_event = 1; 
	evnts.sctp_address_event = 1; 
	evnts.sctp_send_failure_event = 1; 
	evnts.sctp_peer_error_event = 1; 
	evnts.sctp_shutdown_event = 1; 
	evnts.sctp_partial_delivery_event = 1; 
	evnts.sctp_adaptation_layer_event = 1; 
	setsockopt(sock_fd, IPPROTO_SCTP, SCTP_EVENTS, &evnts, sizeof(evnts)); 

	listen(sock_fd, 10); 
	printf("listen.............\n");
	for ( ; ; ) {
		len = sizeof(struct sockaddr_in);
		// print_time_info();
		rd_sz = sctp_recvmsg(sock_fd, readbuf, sizeof(readbuf),(struct sockaddr *)&cliaddr, &len,&sri,&msg_flags);
		if(msg_flags & MSG_NOTIFICATION) {
			printf( "msg_flags (%d),MSG_NOTIFICATION(%d)\n ",msg_flags,MSG_NOTIFICATION);
			// print_notification(readbuf);
			continue;
		}
		sctp_sendmsg(sock_fd, readbuf, rd_sz,(struct sockaddr *)&cliaddr, len,sri.sinfo_ppid,sri.sinfo_flags,sri.sinfo_stream,0, 0);
	}
}
