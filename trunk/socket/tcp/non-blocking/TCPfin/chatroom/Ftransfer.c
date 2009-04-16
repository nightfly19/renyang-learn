#define FPORT 9734
#define FILEBUFFERSIZE 1024
//==========================include file==================================
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Common.h"
int transSer(char* Nfile)
{
	char Ftemp[20];
	bzero(Ftemp, 21);
	strncpy(Ftemp,Nfile,strlen(Nfile)-1);
	
	
	int listenfd,connfd;
	// 在c語言中,要用到struct時,前面必需要加上struct,在c++中則不需要
	struct sockaddr_in server_address;	
	struct sockaddr_in client_address;
	int server_len,client_len;
	char temp[FILEBUFFERSIZE];
	int ReadByte,WriteByte;
	char end[]="#end#";	// 用來判斷是否傳送檔案結束
	FILE *fp;
	
/*	if (argc!=2) {
		printf("Insrtuction error!!\n");
		printf("./server 'file'\n");
		exit(1);
	}
*///	printf("%d",strlen(Ftemp));
	if ((fp=fopen(Ftemp,"rb"))==NULL) {
		fputs("open file error\n",stderr);
		exit(1);
	}
	else 
	//printf("open ok");
	//printf("test");
	// divide - test
	while((ReadByte=fread(temp,sizeof(char),FILEBUFFERSIZE,fp))>0) {
		printf("Read %dBYTE\n",ReadByte);
		memset(temp,0,FILEBUFFERSIZE);
	}
	// divide - test

	// 1.建立一個新的通訊端:socket(int domain,int type,int protocol),並回傳參數值
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	
	// 2.1.設定為Unix network socket,表示可以在兩個不同的電腦之間傳資料
	server_address.sin_family = AF_INET;
	// 2.2.server的ip位址,這裡必需是server的ip位址
	// inet_addr(), this function translate IP address into specified format for socket
	server_address.sin_addr.s_addr=INADDR_ANY;
	// server_address.sin_port = 9734;
	// 2.3.轉換成little-end,因為intel是little-end
	server_address.sin_port = htons(FPORT);
	// 記錄server端的設定(port,ip,sin_family...)的資料長度
	server_len = sizeof(server_address);
	// 3.Socket 出入口需Binding到TCP address
	setsockopt(listenfd,SOL_SOCKET, SO_REUSEADDR, &server_address, sizeof(server_address));
	if(bind(listenfd,(struct sockaddr*) &server_address, server_len)==-1){
		printf("bind error");
		exit(1);
	}
	// 4.建立一個queue以接收其他程式所送達的連線要求,queue的大小為5
	if (listen(listenfd,5)==-1){
		printf("listening error");
		exit(1);
	}
	// 當產生了一個佇列(queue)後，再來就是要把queue中的連結請求(Connect Request)讀出
	while(1)
	{
		// temp存放要傳送的檔案
		char temp[FILEBUFFERSIZE];
		printf("server waiting \n");
		client_len = sizeof(client_address);	// 了解client的設定資料結構長度
		// 5.以accept指令來等待client端送去連線要求(connect request)
		// 由client_address獲得client端的網路位置
		// connfd即由新的connected socket 所存放參數的地方。用來和client 進行資料輸出入
		connfd = accept(listenfd,(struct sockaddr *)&client_address, &client_len);
		if (connfd==-1){
			printf("Error:accept()\n");
			exit(1);
		}
		// 由connfd讀取資料存到temp的位址中,buffer大小為1024 bytes
		read(connfd, temp, FILEBUFFERSIZE);
		if (printf("ch : %s \n",temp)==-1){
			printf("Read error!\n");
				
		}
		else
		{
			if (strncmp(temp,"ok",strlen("ok"))==0)
			{
				printf("File transmite...\n");
				fp=fopen(Ftemp,"rb");
				if (fp==NULL) {
					printf("open file error!!\n");
					exit(1);
				}
				memset(temp,0,sizeof(temp));
				ReadByte=fread(temp,sizeof(char),FILEBUFFERSIZE,fp);
				while(ReadByte>0){
					printf("read %d Byte\n",ReadByte);
fflush(stdout);
					WriteByte=write(connfd,temp,ReadByte);
					printf("transmite %d bytes\n",WriteByte);
fflush(stdout);
					memset(temp,0,sizeof(temp));
					ReadByte=read(connfd,temp,FILEBUFFERSIZE);
					printf("temp=%s\n",temp);
					//printf("test 1\n");
					fflush(stdout);
					ReadByte=fread(temp,sizeof(char),FILEBUFFERSIZE,fp);
				//	printf("test 2\n");
				}

				printf("transition over\n");
fflush(stdout);
				goto end;

			}
			WriteByte=write(connfd,end,sizeof(end));
			printf("Transmite %d bytes\n",WriteByte);
		}
	
		// 關掉connected socket
		close(connfd);
	}
	// 關掉listen的socket
   end:
	//printf("test 3\n");
	close(connfd);
	close(listenfd);

	

	return 0;
}

int transCli(void)
{
	//printf("ip=%s\n",ip);

	int sockfd;
	int ReadByte;
	struct sockaddr_in address;
	int client_len,result;
	FILE *fp;
	//int server_addr;
	char answer[2];
	
/*	if (argc!=2) {
		fputs("./client get",stderr);
		exit(1);
	}
	if((fp=fopen("copy","wb"))==NULL) {
		printf("create error\n");
		exit(1);
	}
	fclose(fp);
*/	//printf("Please input server's ip:");
//	scanf("%s",server_addr);
	// 建立一個socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	address.sin_family = AF_INET;
	// 以下存放的必需是server的ip位址
	address.sin_addr.s_addr=inet_addr("127.0.0.1");
	address.sin_port = htons(FPORT);
	client_len = sizeof(address);
	setsockopt(sockfd,SOL_SOCKET, SO_REUSEADDR, &address, sizeof(address));
	// Client端則呼叫connect()功能,要求與Server主機建立連接通道
	//printf("ip=%s\n",ip);
	//printf("sockfd=%d\n",sockfd);
	//printf("client_len=%d\n",client_len);
	//printf("sockfd=%d\n",sockfd);
	result = connect(sockfd, (struct sockaddr *)&address,client_len);
	
	if(result == -1)	// 若傳回-1則表示連線失敗
	{
		perror("oops: client1");
		fclose(fp);
		exit(1) ;
	}
	//printf("accept the file?\n");
	//scanf("%s",answer);
	//if(answer[0]=='y')
	//{
	printf("accept\n");
	 //傳送資料到server端

	if (write( sockfd, "ok" , FILEBUFFERSIZE)==-1){
		printf("write error\n");
		fclose(fp);
		exit(1);
	}


	else {
		// 接收檔案
		char temp[FILEBUFFERSIZE];
		memset(temp,0,FILEBUFFERSIZE);
		ReadByte=read(sockfd,temp,FILEBUFFERSIZE);
		printf("get the %d bytes\n",ReadByte);
		fp = fopen("copy","wb");
		if (fp==NULL) {
			printf("open file error!!\n");
			exit(1);
		}

		else {
			while(ReadByte>0) {
				if (strncmp(temp,"#end#",strlen("#end#"))==0) {
					printf("The connect is over\n");
					break;
				}
				fwrite(temp,sizeof(char),ReadByte,fp);
				write(sockfd,"I got\n",sizeof("I got\n"));
				memset(temp,0,FILEBUFFERSIZE);
				ReadByte=read(sockfd,temp,FILEBUFFERSIZE);
				//printf("I got the %d bytes\n",ReadByte);
			}
			fclose(fp);
		}
		// 當client接收到資料後,必需要write資料到server才能再read資料!,這樣才叫被動啊!!
	//}
	}

	close( sockfd);	// 關掉socket介面
        
        return 0;
}
