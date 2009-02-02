//==========================include file==================================
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
//========================================================================

//==========================define variable===============================
#define PORT 9734
#define FILEBUFFERSIZE 1024
//========================================================================

//==========================global variable===============================
//========================================================================

//==========================function phototypes===========================
//========================================================================

int main(int argc,char *argv[])
{
	int listenfd,connfd;
	// 在c語言中,要用到struct時,前面必需要加上struct,在c++中則不需要
	struct sockaddr_in server_address;	
	struct sockaddr_in client_address;
	int server_len,client_len;
	FILE *fp;

	if (argc!=2) {
		printf("Insrtuction error!!\n");
		printf("./server 'file'\n");
		return 1;
	}
	if ((fp=fopen(argv[1],"rb"))==NULL) {
		printf("open file error\n");
		return 1;
	}
	// divide - test
	char temp[FILEBUFFERSIZE];
	int ReadByte;
	while((ReadByte=fread(temp,sizeof(char),FILEBUFFERSIZE,fp))>0) {
		printf("%d\n",ReadByte);
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
	server_address.sin_port = htons(PORT);
	// 記錄server端的設定(port,ip,sin_family...)的資料長度
	server_len = sizeof(server_address);
	// 3.Socket 出入口需Binding到TCP address
	if(bind(listenfd,(struct sockaddr*) &server_address, server_len)==-1){
		printf("bind error");
		return 1;
	}
	// 4.建立一個queue以接收其他程式所送達的連線要求,queue的大小為5
	if (listen(listenfd,5)==-1){
		printf("listening error");
		return 1;
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
			return 1;
		}
		// 由connfd讀取資料存到temp的位址中,buffer大小為1024 bytes
		read(connfd, &temp, FILEBUFFERSIZE);
		if (printf("ch : %s \n",temp)==-1){
			printf("Read error!\n");
			return 1;
		}
		else
		{
			if (strncmp(temp,"get",strlen("get"))==0)
			{
				printf("File transmite...\n");
				FILE *fp;
				fp=fopen("original","rb");
				if (fp==NULL) {
					printf("open file error!!\n");
				}
				int ReadByte;
				memset(temp,0,sizeof(temp));
				ReadByte=fread(temp,sizeof(char),FILEBUFFERSIZE,fp);
				printf("read %d Byte\n",ReadByte);
				int WriteByte=write(connfd,temp,ReadByte);
				printf("transmite %d bytes\n",WriteByte);

			}
		}
		// 關掉connected socket
		close(connfd);
	}
	close(listenfd);

	fclose(fp);

	return 0;
}

//==========================function implementation=======================
//========================================================================
