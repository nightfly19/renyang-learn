/*****************************************/
/* Server.c for chat example.*/
/*****************************************/

#include "Server.h"

int main(int argc, char* argv[])
{
	u_short port;
	
	if(argc==2){
		port=(u_short)atoi(argv[1]);
	}else{
		port=2145;
	}		
	
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
