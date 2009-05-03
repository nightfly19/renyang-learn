//======================include file============================
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//==============================================================

//======================define variable=========================
#include "global.h"
//==============================================================

//======================define function=========================
void str_echo(int sockfd);
//==============================================================

//======================main function===========================
int main(int argc,char **argv)
{
	// 建立兩個記錄file descriptor的變數
	// listenfd是server端用來觀察是否有新的連線進來
	// connfd是在server端代表client端的file descriptor
	// 要傳送資訊到client端均是透過connfd
	int listenfd,connfd;
	// childpid表示由主要的main process建立子process的id
	pid_t childpid;
	// servaddr是用來記錄要建立listenfd的server端的相關資訊, 像是使用的協定啦, port號啦, 要建立服務在哪一個ip上啦
	// cliaddr則是用來記錄要建立connfd的client端的相關資料
	// 把它當作一個平台來想像，socket file descriptor是平台上面服務
	struct sockaddr_in cliaddr,servaddr;
	// 宣告一個記錄cliaddr的長度的變數
	socklen_t clilen;
	int ret_value=0;

	// 依所設定的通訊協定建立一個server端的socket
	listenfd = socket(AF_INET,SOCK_STREAM,0);
	if (listenfd == -1) {
		perror("socket error");
		exit(-1);
	}
	// 設定要在哪一個port與哪一個協定，哪一個ip上建立服務
	memset(&servaddr,0,sizeof(servaddr));
	// 使用哪一種協定
	servaddr.sin_family = AF_INET;
	// 要把服務架再所有本機端的ip上
	// 若只要建在某一個ip上可以使用
	// servaddr.sin_addr.s_addr = htonl("a.b.c.d");
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	// 這一個服務要使用哪一個port上
	servaddr.sin_port = htons(SERV_PORT);

	// 把這一個socket服服綁在剛剛建立的平台上
	ret_value = bind(listenfd,(struct sockaddr *) &servaddr,sizeof(servaddr));
	// bind失敗
	if (ret_value == -1) {
		perror("bind error");
		exit(-1);
	}

	// 這一個socket可以提供多少個連線進來
	ret_value = listen(listenfd,LISTENQ);

	for (;;) {
		clilen = sizeof(cliaddr);
		// 等待對方的連線
		// 這個時候cliaddr的內容值是空的
		// 當有連線進來時, 才會依實際上對方的ip...的填入資料
		connfd = accept(listenfd,(struct sockaddr *) &cliaddr,&clilen);

		if ((childpid == fork()) == 0) {	// 等於0表示是child process
			close(listenfd);		// close listening socket
			str_echo(connfd);		// process the request
			exit(0);
		}
		close(connfd);				// parent closes connected socket
	}
	return 0;
}
//==============================================================

//======================function implement======================
void str_echo(int sockfd)
{
	ssize_t n;
	char buf[MAXLINE];
	int ret_value=0;
again:
	// read表示由sockfd，接收到資料，要準備放到buf為起始的位置中, MAXLINE設定為最大接收大小
	while((n = read(sockfd,buf,MAXLINE))>0) {
		// write表示回傳由buf開始的起始位置, 且長度為n的資料送回去給對方
		ret_value = write(sockfd,buf,n);
	}
	if (n<0 && errno==EINTR) {
		printf("n<0 && errno==EINTER\n");
		goto again;
	}
	else
		printf("str_echo: read_error");
}
