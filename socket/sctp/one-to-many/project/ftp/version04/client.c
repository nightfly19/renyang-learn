#include "unp.h"

int main(int argc,char **argv)
{
	int sock_fd;
	struct sockaddr_in servaddr,peeraddr;
	struct sctp_sndrcvinfo sri;
	struct sctp_event_subscribe evnts;
	int msg_flags;
	char readbuf[MAXLINE];
	char filename[FILELEN];
	char filename2[FILELEN];
	socklen_t len;
	bzero(&sri,sizeof(sri));

	if (argc < 2) {
		err_quit("Missing host argument - use '%s host'\n",argv[0]);
	}
	sock_fd = Socket(AF_INET,SOCK_SEQPACKET,IPPROTO_SCTP);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	Inet_pton(AF_INET,argv[1],&servaddr.sin_addr);

	bzero(&evnts,sizeof(evnts));
	evnts.sctp_data_io_event = 1;
	Setsockopt(sock_fd,IPPROTO_SCTP,SCTP_EVENTS,&evnts,sizeof(evnts));
	
	// 接收所輸入的指令
	printf("cmd> ");
	while((fgets(readbuf,MAXLINE,stdin)) != NULL) {
		if (strncmp(readbuf,"\n",1)==0)	{
			printf("cmd> ");
			continue;
		}
		// 若是用fgets，則會在接收的字串最後加入一個\n符號,而不是空白字元,這個時候strlen會把換行符號算進去
		Sctp_sendmsg(sock_fd,readbuf,strlen(readbuf)-1,(SA *) &servaddr,sizeof(servaddr),0,0,sri.sinfo_stream,0,0);
		bzero(filename,sizeof(filename));
		strncpy(filename,readbuf,strlen(readbuf)-1);
		bzero(readbuf,MAXLINE);
		len = sizeof(peeraddr);
		Sctp_recvmsg(sock_fd,readbuf,MAXLINE,(SA *) &peeraddr,&len,&sri,&msg_flags);
		sri.sinfo_stream++;
		if (sri.sinfo_stream>9)
			sri.sinfo_stream = 0;
		if (strncmp(readbuf,"open file ok",strlen("open file ok"))==0) {
			// 取出檔案名稱
			bzero(filename2,sizeof(filename2));
			strcpy(filename2,filename + strlen("#getfile "));
			if (strncmp(filename,"#getfile ",strlen("#getfile "))==0) {
				printf("going to RecvFile\n");
				RecvFile(sock_fd,filename2,(SA *) &peeraddr,sizeof(peeraddr),sri.sinfo_stream);
			}
			else if (strncmp(filename,"#putfile ",strlen("#putfile "))==0) {
				printf("going to SendFile\n");
				SendFile(sock_fd,filename2,(SA *) &peeraddr,sizeof(peeraddr),sri.sinfo_stream);
			}
		}
		else if (strncmp(readbuf,"open file error",strlen("open file error"))==0) {
			printf("file is not exist!!\n");
		}
		printf("cmd> ");
	}

	Close(sock_fd);
	return 0;
}
