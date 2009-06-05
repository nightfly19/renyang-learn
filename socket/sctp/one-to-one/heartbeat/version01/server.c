//===================include header========================
#include "ren_unp.h"
//=========================================================

int main(int argc,char **argv)
{
	int sock_fd,connfd;
	int msg_flags;	// 記錄要傳送訊息時的相關設定,包括association...
	char readbuf[BUFFERSIZE];
	struct sockaddr_in cliaddr, servaddr;
	struct sctp_sndrcvinfo sri;
	struct sctp_event_subscribe evnts;
	socklen_t len;
	size_t rd_sz;	// size_t指的就是unsigned long
	int ret_value=0;
	struct sockaddr *laddrs;	// 用來記錄local addresses
	struct sockaddr *paddrs;
	union sctp_notification *snp;		// 用來轉換傳送過來的訊息
	struct sctp_shutdown_event *sse;	// 當傳送過來的是shutdown event,則可以用這一個指標指到傳送過來的資料
	struct sctp_assoc_change *sac;
	struct sctp_paddr_change *spc;
	struct sctp_remote_error *sre;
	struct sctp_send_failed *ssf;
#ifdef UN_MOD
	struct sctp_adaptation_event *ae;
#else
	struct sctp_adaption_event *ae;
#endif
	struct sctp_pdapi_event *pdapi;
	const char *str;

	if (argc < 2) {
		printf("Error, use %s [list of addresses to bind]\n",argv[0]);
		exit(-1);
	}
	// 建立socket的型態為SCTP的one-to-many的型態
	sock_fd = Socket(AF_INET,SOCK_STREAM, IPPROTO_SCTP);
	if (sock_fd == -1) {
		printf("socket error\n");
		exit(-1);
	}

	// 初始化要bind的server資料,連續加入多個要綁的ip address
	int i;
	for (i=1;i<argc;i++) {
		memset(&servaddr,0,sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		inet_pton(AF_INET,argv[i],&servaddr.sin_addr);	// 把socket與此ip綁在一起
		servaddr.sin_port = htons(SERV_PORT);

		// 把這一個ip與socket綁在一起
		if ((ret_value=sctp_bindx(sock_fd,(struct sockaddr*) &servaddr,1,SCTP_BINDX_ADD_ADDR))==-1) {
			printf("Can't bind the address %s\n",argv[i]);
			exit(-1);
		}
		else
		{
			// 無論如何一定會bind成功,因為,若給錯的ip,則會bind此host端的所有ip
			printf("Bind %s success!!\n",argv[i]);
		}
	}

	// 設定事件
	bzero(&evnts,sizeof(evnts));
	evnts.sctp_data_io_event=1;	// Enable sctp_sndrcvinfo to come with each recvmsg,否則就接收不到對方的資訊了
	evnts.sctp_shutdown_event=1;	// 喔耶!當client端shutdown時,會通知server
	evnts.sctp_association_event=1;	// 監測
	evnts.sctp_address_event = 1;
	evnts.sctp_send_failure_event = 1;
	evnts.sctp_peer_error_event = 1;
	evnts.sctp_partial_delivery_event = 1;
#ifdef UN_MOD
	evnts.sctp_adaptation_layer_event = 1;
#else
	evnts.sctp_adaption_layer_event = 1;
#endif
	ret_value = setsockopt(sock_fd,IPPROTO_SCTP,SCTP_EVENTS,&evnts,sizeof(evnts));
	if (ret_value == -1) {
		printf("setsockopt error\n");
		exit(-1);
	}

	// 設定多少個client可以連線
	ret_value = listen(sock_fd,LISTENQ);
	if (ret_value == -1) {
		printf("listen error\n");
		exit(-1);
	}

	printf("start wait...\n");

	connfd = accept(sock_fd,NULL,NULL);
	if (connfd == -1) {
		printf("accept error\n");
		exit(-1);
	}
	printf("accept the client connecting...\n");

	int init_heart_beat = 0;

	for (;;) {
		memset(readbuf,0,sizeof(readbuf));
		len = sizeof(struct sockaddr_in);
		rd_sz = sctp_recvmsg(connfd,readbuf,sizeof(readbuf),(struct sockaddr *) &cliaddr,&len,&sri,&msg_flags);
		if (init_heart_beat==0)
		{
			heartbeat_action(connfd,(struct sockaddr *) &cliaddr,len,10);
			init_heart_beat = 1;
		}
		//========================================================================
		// test the sctp_getladdrs function - start
		ret_value = sctp_getladdrs(connfd,sri.sinfo_assoc_id,&laddrs);
		printf("The sctp_getladdrs return value is %d\n",ret_value);
		// test the sctp_getladdrs function - end
		// test the sctp_getpaddrs function - start
		ret_value = sctp_getpaddrs(connfd,sri.sinfo_assoc_id,&paddrs);
		printf("The sctp_getpaddrs return value is %d\n",ret_value);
		// test the sctp_getpaddrs function - end
		//========================================================================
		if (msg_flags & MSG_NOTIFICATION) {	// 表示收到一個事件通告,而非一個資料
			snp = (union sctp_notification *) readbuf;
			switch (snp->sn_header.sn_type) {
				case SCTP_SHUTDOWN_EVENT:
					sse = &snp->sn_shutdown_event;
					printf("SCTP_SHUTDOWN_EVENT: assoc=0x%x\n",(uint32_t) sse->sse_assoc_id);
					break;
				case SCTP_ASSOC_CHANGE:
					sac = &snp->sn_assoc_change;
					switch (sac->sac_state) {
						case SCTP_COMM_UP:
							printf("COMMUNICATION UP\n");
							break;
						case SCTP_COMM_LOST:
							printf("COMMUNICATION LOST\n");
							break;
						case SCTP_RESTART:
							printf("RESTART\n");
							break;
						case SCTP_SHUTDOWN_COMP:
							printf("SHUTDOWN COMPLETE\n");
							break;
						case SCTP_CANT_STR_ASSOC:
							printf("CAN'T START ASSOC\n");
							break;
						default:
							printf("UNKNOW\n");
							break;
					}
					break;
				case SCTP_PEER_ADDR_CHANGE:
					spc = &snp->sn_paddr_change;
					switch(spc->spc_state) {
						case SCTP_ADDR_AVAILABLE:
							str = "ADDRESS AVAILABLE";
							break;
						case SCTP_ADDR_UNREACHABLE:
							str = "ADDRESS UNREACHABLE";
							break;
						case SCTP_ADDR_REMOVED:
							str = "ADDRESS REMOVED";
							break;
						case SCTP_ADDR_ADDED:
							str = "ADDRESS ADDED";
							break;
						case SCTP_ADDR_MADE_PRIM:
							str = "ADDRESS MADE PRIMARY";
							break;
						case SCTP_ADDR_CONFIRMED:
							str = "ADDRESS MADE CONFIRMED";
							break;
						default:
							str = "UNKNOW";
							break;
					}
					printf("SCTP_PEER_ADDR_CHANGE: %s\n",str);
					break;
				case SCTP_REMOTE_ERROR:
					sre = &snp->sn_remote_error;
					printf("SCTP_REMOTE_ERROR\n");
					break;
				case SCTP_SEND_FAILED:
					ssf = &snp->sn_send_failed;
					printf("SCTP_SEND_FAILED\n");
					break;
#ifdef UN_MOD
				case SCTP_ADAPTATION_INDICATION:
					ae = &snp->sn_adaptation_event;
					printf("SCTP_ADAPTATION_INDICATION\n");
					break;
#else
				case SCTP_ADAPTION_INDICATION:
					ae = &snp->sn_adaption_event;
					printf("SCTP_ADAPTION_INDICATION\n");
					break;
#endif
				case SCTP_PARTIAL_DELIVERY_EVENT:
					pdapi = &snp->sn_pdapi_event;
					if (pdapi->pdapi_indication == SCTP_PARTIAL_DELIVERY_ABORTED)
						printf("SCTP_PARTIAL_DELIEVERY_ABORTED\n");
					else
						printf("Unknown SCTP_PARTIAL_DELIVERY_EVENT\n");
					break;
				default:
					printf("UNKNOWN notification\n");
					break;
			}
			continue;
		}
		printf("%s",readbuf);
		ret_value = sctp_sendmsg(connfd,readbuf,rd_sz,(struct sockaddr *) &cliaddr,len,sri.sinfo_ppid,sri.sinfo_flags,sri.sinfo_stream,0,0);
		if (ret_value == -1) {
			printf("sctp_sendmsg error\n");
			exit(-1);
		}

	}

	return 0;
}
