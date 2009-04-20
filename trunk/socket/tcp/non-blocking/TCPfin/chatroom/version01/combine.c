#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include "Common.h"

//#define MAX_SERVER_NAME_SIZE 20


u_short port=4242;
//char ip[MAX_SERVER_NAME_SIZE],choice[20];


int main(int argc, char* argv[])
{
//	printf("argc=%d\n",argc);
	
	
		switch(argc){
			case 3:
				strcpy(choice,argv[1]);
				strcpy(ip,argv[2]);
				break;
			case 2:
				strcpy(choice,argv[1]);
				if(!strcmp(choice,"Client"))
				strcpy(ip,"127.0.0.1");
									
				
				break;
			default:
				break;
			}
	
	printf("choice=%s\n",choice);
//	printf("ip=%s\n",ip);


/*********** server or client***************/

	if(!strcmp(choice,"Client")){
	//	printf("choice Client\n");
		
		Clientf(inet_addr(ip));
			}		
	else {
	//	printf("choice Server\n");
		Serverf(&ip);
		
	}

}

int Clientf(int addr){
	/************ Create a Socket ************/
	int ConnectSocket;
	ConnectSocket=socket(PF_INET,SOCK_STREAM,0);
	if(ConnectSocket==-1){
		perror("Error occurred in socket()\n");
		exit(EXIT_FAILURE);
	}
	
	struct hostent *h;

	/*** To determine the arguement is string or integer. ***/
//	if(isalpha(ip[0])){
		
		/*** get IP by host name ***/
/*		h= gethostbyname(ip);
		if(h == NULL){
			printf("Unknown host!\n");
			exit(EXIT_FAILURE);
		}
		addr=*((unsigned long *)h->h_addr);
	}else{
*/		addr=inet_addr(ip);
//	}
	


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

int Serverf(char* ip){

	/************ Create a Socket ************/
	int Server;
	Server=socket(PF_INET,SOCK_STREAM,0);
	if(Server==-1){
		perror("Error occurred in socket()");
		exit(EXIT_FAILURE);
	}
	
	/*********** Inital host address and port *******/
	struct sockaddr_in local;
	
	local.sin_family=AF_INET;
	local.sin_addr.s_addr=INADDR_ANY;
	local.sin_port=htons(port);
	
	/*********** bind Socket *******************/
	if(bind(Server,(struct sockaddr*)&local,sizeof(local)) == -1){
		perror("Error occurred in bind()");
		close(Server);
		exit(EXIT_FAILURE);
	}
	
	/*********** Create socket listener *************/
	if(listen(Server,1)== -1){
		perror("Error occurred in listen()");
		close(Server);
		exit(EXIT_FAILURE);
	}
	
	/***************** Create a SOCKET for accepting incoming ***********/
	int AcceptSocket;
  	printf("#Waiting for client to connect...\n");
	
	while(1){
		struct sockaddr_in sa_client;
		int	addr_len = sizeof(sa_client);
		
		AcceptSocket = accept( Server, (struct sockaddr *)&sa_client, &addr_len );
		if (AcceptSocket == -1){
			perror("Error occurred in accept()");
		}else{
			printf("Connection from %s:%d\n", inet_ntoa(sa_client.sin_addr),sa_client.sin_port);
			chat(AcceptSocket);
			close(AcceptSocket);
		}
		fflush(stdout);
	}
	
	/******** close socket ***************/
	close(Server);
	return 0;
}

