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
	int client_sockfd;
	struct sockaddr_in address;
	int client_len,result;

	// 建立一個socket
	client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	address.sin_family = AF_INET;
	// 以下存放的必需是server的ip位址
	address.sin_addr.s_addr=inet_addr("127.0.0.1");
	address.sin_port = htons(PORT);
	client_len = sizeof(address);
	// Client端則呼叫connect()功能,要求與Server主機建立連接通道
	result = connect(client_sockfd, (struct sockaddr *)&address,client_len);
	if(result == -1)	// 若傳回-1則表示連線失敗
	{
		perror("oops: client1");
		exit(1) ;
	}
	// 傳送資料到server端
	if (write( client_sockfd, &(*argv[1]) , FILEBUFFERSIZE)==-1){
		printf("write error\n");
		exit(1);
	}
	close( client_sockfd);	// 關掉socket介面
        
        return 0;
}
