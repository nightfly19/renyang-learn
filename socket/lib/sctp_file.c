// 還沒有完成
#include "unp.h"

void RecvFile(int sock_fd,char *filename,sockaddr *to,int tolen,int stream_no)
{
	char msgbuf[MAXLINE];
	char readbuf[BUFFSIZE];
	FILE *fp = fopen(filename,"wb");
	struct sockaddr_in peeraddr;
	struct sctp_sndrcvinfo sri;
	int len,msg_flags;
	len = sizeof(peeraddr);
	int rd_sz;
	stream_no++;
	if (stream_no>9)
		stream_no=0;
	if (fp == NULL) {
		Sctp_sendmsg(sock_fd,"open error",strlen("open error"),to,tolen,0,0,stream_no,0,0);
		return ;
	}
	else
	{
		Sctp_sendmsg(sock_fd,"open file ok",strlen("open file ok"),to,tolen,0,0,stream_no,0,0);

		rd_sz = Sctp_recvmsg(sock_fd,readbuf,sizeof(readbuf),(SA *) &peeraddr,&len,&sri,&msg_flags);
		while(strcmp(readbuf,"#over") != 0)
		{
			fwrite(readbuf,1,rd_sz,fp);
			Sctp_sendmsg(sock_fd,"ok",strlen("ok"),&peeraddr,len,0,0,sri.sinfo_stream,0,0);
			rd_sz = Sctp_recvmsg(sock_fd,readbuf,sizeof(readbuf),(SA *) &peeraddr,&len,&sri,&msg_flags);
		}
		fclose(fp);
	}
}

void SendFile(int sock_fd,FILE *fp,sockaddr *to,int tolen,int stream_no)
{
	char sdbuffer[BUFFSIZE];
	char cvbuffer[BUFFSIZE];
	struct sockaddr_in peeraddr;
	int len,msg_flags;
	len = sizeof(peeraddr);
	char end[7] = "#over#";
	int ReadByte = 0;

	while ((ReadByte = fread(sdbuffer,sizeof(char),BUFFSIZE,fp))>0)
	{
		Sctp_sendmsg(sock_fd,sdbuffer,ReadByte,to,tolen,0,0,sri.sinfo_stream,0,0);
		Sctp_recvmsg(sock_fd,cvbuffer,BUFFSIZE,(SA *) &peeraddr,&len,&sri,&msg_flags);
		sri.sinfo_stream++;
		if (sri.sinfo_stream>9)
			sri.sinfo_stream=0;
		bzero(sdbuffer,sizeof(sdbuffer));
	}
	Sctp_sendmsg(sock_fd,"#over#",strlen("#over#"),to,tolen,0,0,sri.sinfo_stream,0,0);
	printf("finish\n");
}
