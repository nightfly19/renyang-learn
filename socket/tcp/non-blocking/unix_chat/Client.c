/*************************************/
/* Client.c for chat example.*/
/*************************************/

#include "Client.h"

int main(int argc, char* argv[])
{
	u_short port;
	char ip[MAX_SERVER_NAME_SIZE];
	
	switch(argc){
		case 3:
			strcpy(ip,argv[1]);
			port=(u_short)atoi(argv[2]);
			break;
		case 2:
			strcpy(ip,argv[1]);
			port=23;
			break;
		case 1:
			strcpy(ip,"127.0.0.1");
			port=23;
			break;
		default:
			printf("#usage: client [host] [port]\n");
			break;
	}
	
	/************ Create a Socket ************/
	int ConnectSocket;
	ConnectSocket=socket(PF_INET,SOCK_STREAM,0);
	if(ConnectSocket==-1){
		perror("Error occurred in socket()\n");
		exit(EXIT_FAILURE);
	}
	
	struct hostent *h;
	unsigned int addr;
	/*** To determine the arguement is string or integer. ***/
	if(isalpha(ip[0])){
		
		/*** get IP by host name ***/
		h= gethostbyname(ip);
		if(h == NULL){
			printf("Unknown host!\n");
			exit(EXIT_FAILURE);
		}
		addr=*((unsigned long *)h->h_addr);
	}else{
		addr=inet_addr(ip);
	}
	
	/*********** Inital host address and port *******/
	struct sockaddr_in server;
	
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=addr;
	server.sin_port=htons(port);
	
	/************ Connect to Server ***************/
	if(connect(ConnectSocket,(struct sockaddr*)&server,sizeof(server))== -1){
		perror("Error occurred in connect()");
		exit(EXIT_FAILURE);	
	}else{
		printf("#Connecting to Server...!\n");
	}
	chat(ConnectSocket);
	
	/******** close socket ***************/
	close(ConnectSocket);
	
	return 0;
}
