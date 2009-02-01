//========================include file====================================
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
//========================================================================


//========================define variable=================================
#define FILEBUFFERSIZE 1024
#define PORT 9734
//========================================================================

//========================global variable=================================
//========================================================================

//========================function phototypes=============================
//========================================================================

int main(int argc,char *argv[])
{
//=============================================================
	int server_sockfd,client_sockfd;
	// 在c語言中,要用到struct時,前面必需要加上struct,在c++中則不需要
	struct sockaddr_in server_address;	
	struct sockaddr_in client_address;
	int server_len,client_len;

	// 1.建立一個新的通訊端:socket(int domain,int type,int protocol),並回傳參數值
	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
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
	if(bind(server_sockfd,(struct sockaddr*) &server_address, server_len)==-1){
		printf("bind error");
		return 1;
	}
	// 4.建立一個queue以接收其他程式所送達的連線要求,queue的大小為5
	if (listen(server_sockfd,5)==-1){
		printf("listening error");
		return 1;
	}
	// 當產生了一個佇列(queue)後，再來就是要把queue中的連結請求(Connect Request)讀出
	while(1)
	{
		char temp[1024];
		memset(temp,0,FILEBUFFERSIZE);
		printf("server waiting \n");
		client_len = sizeof(client_address);	// 了解client的設定資料結構長度
		// 5.以accept指令來等待client端送去連線要求(connect request)
		// 由client_address獲得client端的網路位置
		// client_sockfd即由新的connected socket 所存放參數的地方。用來和client 進行資料輸出入
		client_sockfd = accept(server_sockfd,(struct sockaddr *)&client_address, &client_len);
		if (client_sockfd==-1){
			printf("Error:accept()\n");
			return 1;
		}
		// 由client_sockfd讀取資料存到temp的位址中,buffer大小為10byte
		/*
		read(client_sockfd, &temp, FILEBUFFERSIZE);
		if (printf("ch : %s \n",temp)==-1){
			printf("Read error!\n");
			return 1;
		}*/
		read(client_sockfd,&temp,FILEBUFFERSIZE);
		if (strcmp(temp,"get")==0)
		{
			FILE *fp;
			fp=fopen("original","rb");
			if (fp==NULL)
			{
				printf("開啟檔案錯誤\n");
				return 1;
			}
			else
			{
				int ReadByte;
				while((ReadByte=fread(temp,sizeof(char),FILEBUFFERSIZE,fp))>0)
				{
					// 傳送檔案
					write(client_sockfd,&temp,ReadByte);
					memset(temp,0,FILEBUFFERSIZE);
				}
			}
		}
		strcpy(temp,"exit");
		write(client_sockfd,&temp,4);
		// 關掉connected socket
		close(client_sockfd);
	}
	close(server_sockfd);
	return 0;
}
//========================function implementation====================================
//===================================================================================
