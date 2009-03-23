//===================include header========================
#include "ren_unp.h"
//=========================================================

int main(int argc,char **argv)
{
	int sock_fd;
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

	if (argc < 2) {
		printf("Error, use %s [list of addresses to bind]\n",argv[0]);
		exit(-1);
	}
	// 建立socket的型態為SCTP的one-to-many的型態
	sock_fd = Socket(AF_INET,SOCK_SEQPACKET, IPPROTO_SCTP);
	if (sock_fd == -1) {
		printf("socket error\n");
		exit(-1);
	}

	// 初始化要bind的server資料
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET,argv[1],&servaddr.sin_addr);	// 把socket與此ip綁在一起
	servaddr.sin_port = htons(SERV_PORT);

	// 把這一個ip與socket綁在一起
	if ((ret_value=sctp_bindx(sock_fd,(struct sockaddr*) &servaddr,1,SCTP_BINDX_ADD_ADDR))==-1) {
		printf("Can't bind the address set\n");
		exit(-1);
	}
	else
	{
		// 無論如何一定會bind成功,因為,若給錯的ip,則會bind此host端的所有ip
		printf("Bind success!!\n");
	}

	// 設定事件
	bzero(&evnts,sizeof(evnts));
	evnts.sctp_data_io_event=1;
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
	for (;;) {
		len = sizeof(struct sockaddr_in);
		rd_sz = sctp_recvmsg(sock_fd,readbuf,sizeof(readbuf),(struct sockaddr *) &cliaddr,&len,&sri,&msg_flags);
		//========================================================================
		// test the sctp_getladdrs function - start
		ret_value = sctp_getladdrs(sock_fd,sri.sinfo_assoc_id,&laddrs);
		printf("The sctp_getladdrs return value is %d\n",ret_value);
		// test the sctp_getladdrs function - end
		// test the sctp_getpaddrs function - start
		ret_value = sctp_getpaddrs(sock_fd,sri.sinfo_assoc_id,&paddrs);
		printf("The sctp_getpaddrs return value is %d\n",ret_value);
		// test the sctp_getpaddrs function - end
		//========================================================================
		if (msg_flags & MSG_NOTIFICATION) {
			printf("%s\n",readbuf);
			continue;
		}
		printf("%s",readbuf);
		ret_value = sctp_sendmsg(sock_fd,readbuf,rd_sz,(struct sockaddr *) &cliaddr,len,sri.sinfo_ppid,sri.sinfo_flags,sri.sinfo_stream,0,0);
		if (ret_value == -1) {
			printf("sctp_sendmsg error\n");
			exit(-1);
		}

	}

	return 0;
}
