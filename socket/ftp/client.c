#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

#define FILEBUFFERSIZE 1024

int main(int argc,char *argv[])
{
	
	printf("dd");
	int client_sockfd;
	struct sockaddr_in address;
	int client_len,result;

	// 建立一個socket
	client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	address.sin_family = AF_INET;
	// 以下存放的必需是server的ip位址
	address.sin_addr.s_addr=inet_addr("127.0.0.1");
	address.sin_port = htons(9734);
	client_len = sizeof(address);
	// Client端則呼叫connect()功能,要求與Server主機建立連接通道
	result = connect(client_sockfd, (struct sockaddr *)&address,client_len);
	if(result == -1)	// 若傳回-1則表示連線失敗
	{
		perror("oops: client1");
		return ;
	}
	// 傳送資料到server端,buffer為10 bytes
	if (strcmp(argv[1],"get")!=0)
	{
		return 1;
	}
	if (write( client_sockfd, &(*argv[1]) , 1024)==-1){
		printf("write error\n");
		return 1;
	}
	FILE *fp = fopen("copy","wb");
	if (fp==NULL){
		printf("檔案開啟錯誤\n");
		close(client_sockfd);
		return 1;
	}
	char temp[1024];
	while(strcmp(temp,"exit")!=0)
	{
		memset(temp,0,1024);
		read(client_sockfd, &temp, FILEBUFFERSIZE);
		if (strcmp(temp,"exit")==0)
			break;
		fwrite(temp,sizeof(char),FILEBUFFERSIZE,fp);
	}
	close( client_sockfd);	// 關掉socket介面
        
        return 0;
}
