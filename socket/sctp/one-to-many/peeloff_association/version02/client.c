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
#define MAXLINE 4096
#define SCTP_MAXLINE 800
#define SERV_MAX_SCTP_STRM 10
//=============================================================================

//==============================define function================================
void sctpstr_cli(FILE *fp,int sock_fd,struct sockaddr *to,socklen_t tolen);
//=============================================================================

//==============================main function==================================
int main(int argc,char **argv)
{
	int sock_fd;	// client所建立的socket
	struct sockaddr_in servaddr;	// 記錄server的sockaddr_in的相關資訊
	struct sctp_event_subscribe evnts;	// 設定要觀查的事件
	int ret_value = 0;
	
	// 沒有設定server的ip
	if (argc < 2) {
		printf("Missing host argument - use '%s host'\n",argv[0]);
		exit(-1);
	}

	// init setting
	sock_fd = socket(AF_INET,SOCK_SEQPACKET,IPPROTO_SCTP);	// 設定為sctp為one-to-many的型態
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);	// 感覺這一行應該是多餘的
	servaddr.sin_port = htons(SERV_PORT);
	ret_value = inet_pton(AF_INET,argv[1],&servaddr.sin_addr);	// 把字串轉換成網路位元組序二進位值,當回傳為1時,表轉換成功
	if (ret_value != 1) {
		printf("translate server ip address error!!\n");
		exit(-1);
	}

	// 設定要觀察的事件,所有都清除,只設定io event
	memset(&evnts,0,sizeof(evnts));
	evnts.sctp_data_io_event = 1;	// 監控sctp的data的io事件:1是打開的意思
	ret_value = setsockopt(sock_fd,IPPROTO_SCTP,SCTP_EVENTS,&evnts,sizeof(evnts));
        if (ret_value == -1) {
                printf("setsockopt error\n");
                exit(-1);
        }
	// 此client只傳給一個server
	sctpstr_cli(stdin,sock_fd,(struct sockaddr *) &servaddr,sizeof(servaddr));
	close(sock_fd);
	return 0;
}
//=============================================================================

//==============================function implemen==============================
void sctpstr_cli(FILE *fp,int sock_fd,struct sockaddr *to,socklen_t tolen)
{
	struct sockaddr_in peeraddr;	// client的相關位址資料
	struct sctp_sndrcvinfo sri;	// send and recv端的資料
	char sendline[MAXLINE],recvline[MAXLINE];
	socklen_t len;
	int out_sz,rd_sz;
	int msg_flags;
	int ret_value=0;
	int conn_fd = -1;

	bzero(&sri,sizeof(sri));
	// block住,等待client端輸入
	while (fgets(sendline,MAXLINE,fp) != NULL) {
		if (sendline[0] != '[') {
			printf("Error, line must be of the form '[streamnum]text'\n");
			continue;
		}
		// string to long integer,表示sendline這一個字串是base在10進位去轉換
		// 透過設定的stream number來傳送資料
		sri.sinfo_stream = strtol(&sendline[1],NULL,0);
		out_sz = strlen(sendline);	// 輸入字串的長度,用來記錄等一下要傳送幾個字元

		// 傳送
		if (conn_fd == -1)
		{
			ret_value = sctp_sendmsg(sock_fd,sendline,out_sz,to,tolen,0,0,sri.sinfo_stream,0,0);
		}
		else
		{
			ret_value = sctp_sendmsg(conn_fd,sendline,out_sz,to,tolen,0,0,sri.sinfo_stream,0,0);
		}
		if (ret_value == -1) {
			printf("sctp_sendmsg error\n");
			exit(-1);
		}

		len = sizeof(peeraddr);
		if (conn_fd == -1)
		{
			rd_sz = sctp_recvmsg(sock_fd,recvline,sizeof(recvline),(struct sockaddr *) &peeraddr,&len,&sri,&msg_flags);
			if (rd_sz == -1) {
				printf("sctp_recvmsg error\n");
				exit(-1);
			}
			if ((conn_fd = sctp_peeloff(sock_fd,(u_int) sri.sinfo_assoc_id))==-1)
				printf("sctp_peeloff fail!!\n");
			else
			{
				printf("the new socket is %d\n",conn_fd);
				printf("sctp_peeloff success\n");
			}
		}
		else
		{
			rd_sz = sctp_recvmsg(conn_fd,recvline,sizeof(recvline),(struct sockaddr *) &peeraddr,&len,&sri,&msg_flags);
			if (rd_sz == -1) {
				printf("sctp_recvmsg error\n");
				exit(-1);
			}
		}
		printf("From str:%d seq:%d (assoc:0x%x):",sri.sinfo_stream,sri.sinfo_ssn,(u_int) sri.sinfo_assoc_id);
		printf("%.*s",rd_sz,recvline);
	}
}
