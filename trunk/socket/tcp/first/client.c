//============================include file===================================
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//===========================================================================

//=============================define variable===============================
#define PORT 9734
#define FILEBUFFERSIZE 1024
//===========================================================================

int main(int argc,char *argv[])
{
	int sockfd;
	int ReadByte;
	struct sockaddr_in address;
	int client_len,result;
	FILE *fp;
	char server_addr[15];
	
	if (argc!=2) {
		fputs("./client get",stderr);
		exit(1);
	}
	if((fp=fopen("copy","wb"))==NULL) {
		printf("create error\n");
		exit(1);
	}
	fclose(fp);
	printf("Please input server's ip:");
	scanf("%s",server_addr);
	// 建立一個socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	address.sin_family = AF_INET;
	// 以下存放的必需是server的ip位址
	address.sin_addr.s_addr=inet_addr(server_addr);
	address.sin_port = htons(PORT);
	client_len = sizeof(address);
	// Client端則呼叫connect()功能,要求與Server主機建立連接通道
	result = connect(sockfd, (struct sockaddr *)&address,client_len);
	if(result == -1)	// 若傳回-1則表示連線失敗
	{
		perror("oops: client1");
		fclose(fp);
		exit(1) ;
	}
	// 傳送資料到server端
	if (write( sockfd, &(*argv[1]) , FILEBUFFERSIZE)==-1){
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
				printf("I got the %d bytes\n",ReadByte);
			}
			fclose(fp);
		}
		// 當client接收到資料後,必需要write資料到server才能再read資料!,這樣才叫被動啊!!
	}
	close( sockfd);	// 關掉socket介面
        
        return 0;
}
