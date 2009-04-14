#include "unp.h"

int main(int argc,char **argv)
{
	int sock_fd,msg_flags;
	char readbuf[BUFFSIZE];
	char filename[BUFFSIZE];
	char filename2[BUFFSIZE];
	struct sockaddr_in servaddr, cliaddr;
	struct sctp_sndrcvinfo sri;
	struct sctp_event_subscribe evnts;
	FILE *fp;
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
		sri.sinfo_stream++;
		if (sri.sinfo_stream>9)
			sri.sinfo_stream=0;
		printf("You input is '%s'\n",readbuf);
		if (strncmp(readbuf,"#getfile ",strlen("#getfile "))==0) {
			bzero(filename,BUFFSIZE);
			strcpy(filename,readbuf + strlen("#getfile "));	// 取出檔案名稱
			printf("goto Sctp_sendmsg\n");
			SendFile(sock_fd,filename,(SA *) &cliaddr,sizeof(cliaddr),sri.sinfo_stream);
			continue;
		}
		else if (strncmp(readbuf,"#putfile ",strlen("#putfile "))==0) {
			bzero(filename,BUFFSIZE);
			strcpy(filename,readbuf + strlen("#putfile "));
			fp = fopen(filename,"wb");
			if (fp ==NULL) {
				Sctp_sendmsg(sock_fd,"open file error",strlen("open file error"),(SA *) &cliaddr,sizeof(cliaddr),0,0,sri.sinfo_stream,0,0);
			}
			else {
				Sctp_sendmsg(sock_fd,"open file ok",strlen("open file ok"),(SA *) &cliaddr,sizeof(cliaddr),0,0,sri.sinfo_stream,0,0);
				fclose(fp);
			}
			Sctp_recvmsg(sock_fd,readbuf,sizeof(readbuf),(SA *) &cliaddr,&len,&sri,&msg_flags);
			if (strncmp(readbuf,"open file ok",strlen("open file ok"))!=0) {
				Sctp_sendmsg(sock_fd,"peer open error",strlen("peer open error"),(SA *) &cliaddr,sizeof(cliaddr),0,0,sri.sinfo_stream,0,0);
				continue;
			}
			printf("goto Sctp_recvmsg\n");
			RecvFile(sock_fd,filename,(SA *) &cliaddr,sizeof(cliaddr),sri.sinfo_stream);
			continue;
		}
		else {
			Sctp_sendmsg(sock_fd,"ok",strlen("ok"),(SA *) &cliaddr,sizeof(cliaddr),0,0,sri.sinfo_stream,0,0);
		}
		bzero(&readbuf,sizeof(readbuf));
	}
	return 0;
}
