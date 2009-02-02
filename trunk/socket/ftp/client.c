//============================include file===================================
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
//===========================================================================

//=============================define variable===============================
#define PORT 9734
#define FILEBUFFERSIZE 1024
//===========================================================================

int main(int argc,char *argv[])
{
	int sockfd;
	struct sockaddr_in address;
	int client_len,result;
	FILE *fp;
	
	if((fp=fopen("copy","wb"))==NULL) {
		printf("create error\n");
		return 1;
	}
	// 建立一個socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	address.sin_family = AF_INET;
	// 以下存放的必需是server的ip位址
	address.sin_addr.s_addr=inet_addr("127.0.0.1");
	address.sin_port = htons(PORT);
	client_len = sizeof(address);
	// Client端則呼叫connect()功能,要求與Server主機建立連接通道
	result = connect(sockfd, (struct sockaddr *)&address,client_len);
	if(result == -1)	// 若傳回-1則表示連線失敗
	{
		perror("oops: client1");
		fclose(fp);
		return 1 ;
	}
	// 傳送資料到server端
	if (write( sockfd, &(*argv[1]) , FILEBUFFERSIZE)==-1){
		printf("write error\n");
		fclose(fp);
		return 1;
	}
	else {
		// 接收檔案
	}
	close( sockfd);	// 關掉socket介面
	fclose(fp);
        
        return 0;
}
