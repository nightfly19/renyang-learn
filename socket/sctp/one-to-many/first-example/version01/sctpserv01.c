//==============================include header file============================
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//=============================================================================

//==============================define variable================================
#define BUFFERSIZE 8192
#define SERV_PORT 9877
#define LISTENQ 1024
//=============================================================================

int main(int argc,char **argv)
{
	int echo_to_all = 0;

	int ret_value;
	int sock_fd;
	int msg_flags;	// 記錄要傳送訊息時的相關設定
	char readbuf[BUFFERSIZE];
	struct sockaddr_in servaddr,cliaddr;
	struct sctp_sndrcvinfo sri;	// 記錄send, recv, association的相關資訊
	struct sctp_event_subscribe evnts;
	int stream_increment = 1;
	socklen_t len;
	size_t rd_sz;	// size_t指的就是unsigned long:rd_sz表示recv,send-size

	if (argc == 2) {
		// 把字串轉換成數字
		stream_increment = atoi(argv[1]);
	}

	// 建立socket的型態為SCTP的one-to-many的型態
	sock_fd = socket(AF_INET,SOCK_SEQPACKET,IPPROTO_SCTP);
	if (sock_fd == -1) {
		printf("socket error\n");
		exit(-1);
	}
	// 初始化server address的設定
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	
	// 把socket與所建立的address綁在一起
	ret_value = bind(sock_fd,(struct sockaddr *) &servaddr,sizeof(servaddr));
	if (ret_value == -1) {
		printf("bind error\n");
		exit(-1);
	}

	// Set up for notifications of internet
	bzero(&evnts,sizeof(evnts));
	evnts.sctp_data_io_event = 1;	// 監控sctp的data的io事件:1是打開的意思
	ret_value = setsockopt(sock_fd,IPPROTO_SCTP,SCTP_EVENTS,&evnts,sizeof(evnts));
	if (ret_value == -1) {
		printf("setsockopt error\n");
		exit(-1);
	}

	// 設定等待多少個client端連線
	ret_value = listen(sock_fd,LISTENQ);
	if (ret_value == -1) {
		printf("listen error\n");
		exit(-1);
	}

	printf("start wait...\n");
	for (;;) {
		len = sizeof(struct sockaddr_in);
		// 等待client端連線
		rd_sz = sctp_recvmsg(sock_fd,readbuf,sizeof(readbuf),(struct sockaddr *) &cliaddr,&len,&sri,&msg_flags);
		if (stream_increment) {
			sri.sinfo_stream++;
			if (sri.sinfo_stream >= 100) {
				sri.sinfo_stream = 0;
			}
		}
		// 把接收到的資料回送給client
		ret_value = sctp_sendmsg(sock_fd,readbuf,rd_sz,(struct sockaddr *) &cliaddr,len,sri.sinfo_ppid,sri.sinfo_flags,sri.sinfo_stream,0,0);
		if (ret_value == -1) {
			printf("sctp_sendmsg error\n");
			exit(-1);
		}
	}
	return 0;
}
