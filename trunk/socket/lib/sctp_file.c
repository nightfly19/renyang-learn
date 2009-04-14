// 還沒有完成
#include "unp.h"

// 接收的一方要先建立同樣檔名的檔案,然後,先等待
void RecvFile(int sock_fd,char *filename,struct sockaddr *to,int tolen,int stream_no)
{
	char msgbuf[MAXLINE];	// 用來接收指令的
	char readbuf[BUFFSIZE];	// 用來接收檔案的
	FILE *fp;
	struct sockaddr_in peeraddr;
	struct sctp_sndrcvinfo sri;
	int len,msg_flags;
	len = sizeof(peeraddr);
	int ReadByte;
	fp = fopen(filename,"wb");	// 是否可以建立一個要寫入的檔案
	if (fp == NULL) {		// 檔案建立失敗,傳送錯誤訊息
		Sctp_sendmsg(sock_fd,"open file error",strlen("open file error"),to,tolen,0,0,stream_no,0,0);
		return ;
	}
	else	// 表示建立要寫入的檔案成功,準備開始要接收檔案的程序啦
	{
		// 告知對方我準備好啦~
		Sctp_sendmsg(sock_fd,"open file ok",strlen("open file ok"),to,tolen,0,0,stream_no,0,0);

		// 準備開始接收檔案,ReadByte表示接收到的字元
		ReadByte = Sctp_recvmsg(sock_fd,readbuf,sizeof(readbuf),(SA *) &peeraddr,&len,&sri,&msg_flags);

		// 判斷檔案是否為自定的結束符號
		printf("start receiving");
		while(strncmp(readbuf,"#over#",strlen("#over#")) != 0)
		{
			printf(".");
			fflush(stdout);
			// 把接收到的部分存到檔案中
			fwrite(readbuf,sizeof(char),ReadByte,fp);
			// 表示我有接收到啦,請傳下一個部分
			Sctp_sendmsg(sock_fd,"ok",strlen("ok"),(SA *)&peeraddr,sizeof(peeraddr),0,0,sri.sinfo_stream,0,0);
			// 接收下一個部分的檔案
			ReadByte = Sctp_recvmsg(sock_fd,readbuf,sizeof(readbuf),(SA *) &peeraddr,&len,&sri,&msg_flags);

			// 增加stream no,預設值不可以大於9
			sri.sinfo_stream++;
			if (sri.sinfo_stream>9)
				sri.sinfo_stream=0;
		}
		printf("\nreceiving finish!!\n");
		Fclose(fp);
	}
}

void SendFile(int sock_fd,char *filename,struct sockaddr *to,int tolen,int stream_no)
{
	char msgbuf[MAXLINE];	// 用來接收指令的
	char readbuf[BUFFSIZE];	// 用來接收檔案的
	char sdbuffer[BUFFSIZE];// 用來傳送資料的
	struct sockaddr_in peeraddr;
	struct sctp_sndrcvinfo sri;
	int len,msg_flags;
	char end[7] = "#over#";
	int ReadByte = 0;
	FILE *fp;
	len = sizeof(peeraddr);
	bzero(sdbuffer,BUFFSIZE);
	if ((fp=fopen(filename,"rb")) == NULL) {
		Sctp_sendmsg(sock_fd,"open file error",strlen("open file error"),to,tolen,0,0,sri.sinfo_stream,0,0);
		return ;
	}
	else {
		Sctp_sendmsg(sock_fd,"open file ok",strlen("open file ok"),to,tolen,0,0,sri.sinfo_stream,0,0);
	}
	
	ReadByte = Sctp_recvmsg(sock_fd,msgbuf,sizeof(msgbuf),(SA *) &peeraddr,&len,&sri,&msg_flags);
	if (strncmp(msgbuf,"open file ok",strlen("open file ok"))==0) {
		printf("sending");
		while ((ReadByte = fread(sdbuffer,sizeof(char),BUFFSIZE,fp))>0)
		{
			// 顯示進度
			printf(".");
			fflush(stdout);
			// 把由檔案讀出來的資料傳送出去
			Sctp_sendmsg(sock_fd,sdbuffer,ReadByte,to,tolen,0,0,sri.sinfo_stream,0,0);

			// 以下這一個指令只是用來等待對方確定接收完，再傳送下一個資料片斷
			Sctp_recvmsg(sock_fd,msgbuf,MAXLINE,(SA *) &peeraddr,&len,&sri,&msg_flags);
			sri.sinfo_stream++;
			if (sri.sinfo_stream>9)
				sri.sinfo_stream=0;
			// 把由檔案中讀到並且已送出去的資料清空
			bzero(sdbuffer,sizeof(sdbuffer));
			// 準備讀取下一個檔案片斷
		}
	}
	else
	{
		printf("receive endpoint return error\n");
		Fclose(fp);
		return ;
	}
	Sctp_sendmsg(sock_fd,"#over#",strlen("#over#"),to,tolen,0,0,sri.sinfo_stream,0,0);
	printf("\nsending finish\n");
	Fclose(fp);
}
