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
void str_cli(FILE *fp,int sockfd);
//==============================================================

//======================main function===========================
int main(int argc,char **argv)
{
	int sockfd;
	// servaddr代表在client端表示server端的socket
	struct sockaddr_in servaddr;
	int ret_value=0;

	if (argc != 2) {
		printf("usage: tcpcli <IPaddress>");
		exit(-1);
	}

	// 建立一個服務
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if (sockfd == -1) {
		printf("socket error\n");
		exit(-1);
	}

	// 填入server端的相關資料, ip address, port number
	bzero(&servaddr,sizeof(servaddr));
	// 填入server端要使用的協定
	servaddr.sin_family = AF_INET;
	// 填入server端要使用的port號
	servaddr.sin_port = htons(SERV_PORT);
	// 把string形式的ip address轉換成network的形式
	ret_value = inet_pton(AF_INET,argv[1],&servaddr.sin_addr);
	if (ret_value == -1) {
		printf("inet_pton error\n");
		exit(-1);
	}

	// 連線到server端
	ret_value = connect(sockfd,(struct sockaddr *) &servaddr,sizeof(servaddr));
	if (ret_value == -1) {
		perror("connect error\n");
		exit(-1);
	}

	str_cli(stdin,sockfd);

	exit(0);
}
//==============================================================

//======================function implement======================
void str_cli(FILE *fp,int sockfd)
{
	char sendline[MAXLINE],recvline[MAXLINE];
	int ret_value=0;

	// 抓由使用者輸入的資料
	while (fgets(sendline,MAXLINE,fp) != NULL) {
		// 傳送以sendline記憶體位址為起點長度為strlen(sendline)到server端
		ret_value = write(sockfd,sendline,strlen(sendline));

		memset(recvline,0,sizeof(recvline));
		// 由server端接收資料, 並寫入到記憶體位置recvline, 並限定最大長度為MAXLINE
		if (read(sockfd,recvline,MAXLINE) ==0) {
			printf("str_cli: server terminated prematurely");
			exit(-1);
		}

		// 把資料送到使用者面前
		fputs(recvline,stdout);
	}
}
