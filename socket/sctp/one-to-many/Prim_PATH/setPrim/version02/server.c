#include	"unp.h"

int
main(int argc, char **argv)
{
	int sock_fd,msg_flags;
	char readbuf[BUFFSIZE];
	char *retbuf;
	struct sockaddr_in servaddr, cliaddr;
	struct sctp_sndrcvinfo sri;
	struct sctp_event_subscribe evnts;
	int stream_increment=1;
	socklen_t len;
	size_t rd_sz;
	int ret;
	int maxfd = -1;
	fd_set rfds;


	if (argc == 2)
		stream_increment = atoi(argv[1]);
        sock_fd = Socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	Bind(sock_fd, (SA *) &servaddr, sizeof(servaddr));
	
/* include mod_serv06 */
	bzero(&evnts, sizeof(evnts));
	evnts.sctp_data_io_event = 1;
	evnts.sctp_association_event = 1;
	evnts.sctp_address_event = 1;
	evnts.sctp_send_failure_event = 1;
	evnts.sctp_peer_error_event = 1;
	evnts.sctp_shutdown_event = 1;
	evnts.sctp_partial_delivery_event = 1;
#ifdef UN_MOD
	evnts.sctp_adaptation_layer_event = 1;
#else
	evnts.sctp_adaption_layer_event = 1;
#endif
	Setsockopt(sock_fd, IPPROTO_SCTP, SCTP_EVENTS,
		   &evnts, sizeof(evnts));

	Listen(sock_fd, LISTENQ);


	printf("Start waiting...\n");
	for ( ; ; ) {
		// for select - start
		FD_ZERO(&rfds);
		FD_SET(0,&rfds);
		maxfd = 0;
		FD_SET(sock_fd,&rfds);
		if (sock_fd > maxfd) {
			maxfd = sock_fd;
		}
		// for select - end
		len = sizeof(struct sockaddr_in);
		ret = select(maxfd + 1,&rfds,NULL,NULL,NULL);
		if (ret == -1) {
			printf("Select () Error:%s\n",strerror(errno));
		}
		if (FD_ISSET(0,&rfds)) {
			printf("You input something\n");
			bzero(readbuf,sizeof(readbuf));
			fgets(readbuf,sizeof(readbuf),stdin);
			Sctp_sendmsg(sock_fd, readbuf, rd_sz,(SA *) &cliaddr,len,sri.sinfo_ppid,sri.sinfo_flags,sri.sinfo_stream,0,0);
			continue;
		}
		
		if (!(FD_ISSET(sock_fd,&rfds))) {
			continue;
		}
		rd_sz = Sctp_recvmsg(sock_fd, readbuf, sizeof(readbuf),
			     (SA *)&cliaddr, &len,
			     &sri,&msg_flags);
		if(msg_flags & MSG_NOTIFICATION) {	// 表示收到一個事件,而非一個資料
			print_notification(readbuf);
			continue;
		}
		// 取得指定的association的primary ip address
		if ((retbuf = sctp_getprim(sock_fd,(u_int)sri.sinfo_assoc_id)) !=NULL)
			printf("The local primary ip is %s\n",retbuf);
		else
			err_ret("Error:Get prim address\n");
/* end mod_serv06 */
		if(stream_increment) {
			sri.sinfo_stream++;
			// getsockopt用在sctp有問題!!先跳過!
			// if(sri.sinfo_stream >= sctp_get_no_strms(sock_fd,(SA *)&cliaddr, len)) 
			if(sri.sinfo_stream >= 100)
				sri.sinfo_stream = 0;
		}
		Sctp_sendmsg(sock_fd, readbuf, rd_sz, 
			     (SA *)&cliaddr, len,
			     sri.sinfo_ppid,
			     sri.sinfo_flags,
			     sri.sinfo_stream,
			     0, 0);
	}
}
