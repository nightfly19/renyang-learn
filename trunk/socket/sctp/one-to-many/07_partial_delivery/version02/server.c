#include	"unp.h"

int
main(int argc, char **argv)
{
	// 注意,這裡的readbuf是uint8_t的指標
	uint8_t *readbuf;
	int sock_fd,msg_flags;
	struct sockaddr_in servaddr, cliaddr;
	struct sctp_sndrcvinfo sri;	// 記錄send,recv,association的相關資訊
	struct sctp_event_subscribe evnts;
	int stream_increment=1;
	socklen_t len;
	size_t rd_sz;	// size_t指的就是unsigned long

	if (argc == 2)
		stream_increment = atoi(argv[1]);
	// 建立socket的型態為SCTP的one-to-many的型態
        sock_fd = Socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
	/*
        // 預設為10條stream,設定為30條
        memset(&initm,0,sizeof(initm));
        initm.sinit_num_ostreams = SERV_MORE_STRMS_SCTP;
        Setsockopt(sock_fd,IPPROTO_SCTP,SCTP_INITMSG,&initm,sizeof(initm));
	*/
	// 初始化server address設定
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	// 把socket與所建立的address綁在一起
	Bind(sock_fd, (SA *) &servaddr, sizeof(servaddr));
	
	// Enable sctp_sndrcvinfo to come with each recvmsg
	bzero(&evnts, sizeof(evnts));
	evnts.sctp_data_io_event = 1;	// 1是打開的意思
	Setsockopt(sock_fd, IPPROTO_SCTP, SCTP_EVENTS,
		   &evnts, sizeof(evnts));

	// 設定等待多少個client端的連線
	Listen(sock_fd, LISTENQ);
	printf("start waiting...\n");
	rd_sz = 0;
/* include mod_serv05 */
	for ( ; ; ) {
		len = sizeof(struct sockaddr_in);
		bzero(&sri,sizeof(sri));
		// 等待client端連線
		readbuf = pdapi_recvmsg(sock_fd, &rd_sz,
					(SA *)&cliaddr, &len,
					&sri,&msg_flags);
		if(readbuf == NULL)
		   continue;
/* end mod_serv05 */
		if(stream_increment) {
			sri.sinfo_stream++;
			// 在使用sctp_get_no_stream時,對sctp使用getsockopt有出現問題
			// if(sri.sinfo_stream >= sctp_get_no_strms(sock_fd,(SA *)&cliaddr, len)) 
			if (sri.sinfo_stream >= 100)
				sri.sinfo_stream = 0;
		}
		// 把接收到的資料回送給client
		Sctp_sendmsg(sock_fd, readbuf, rd_sz, 
			     (SA *)&cliaddr, len,
			     sri.sinfo_ppid,
			     sri.sinfo_flags,
			     sri.sinfo_stream,
			     0, 0);
	}
}
