#include <stdio.h>
#include <stdlib.h>

#include "Server.h"
#include "Client.h"

int main(int argc, char* argv[])
{
	int port = 2345;
	
	switch(argc){
		case 1:
			port = 2345;
			break;
		case 2:
			port = atoi(argv[1]);
			break;
		default:
			printf("syntax error: server [port]\n");
			break;		
	}
	
	int ret;
	char buf[MAX_BUF];	
	Server server(port);
	Client client;
	
	while(true){
		client = server.accept();
		
		try{
			ret = client.send("Hello! I'm C++ Server.\n");
			if(ret <= 0 ) throw ret;
		
		}catch(int err){
	
			if(err == 0) {
				client.close();
				printf("Client is offline!\n");
			}else if(err == -1) printf("Fail to send message to Client!\n");
		}
		
		try{
			ret = client.recv(buf);
			if(ret <= 0) throw ret;
			printf("%s", buf);
				
		}catch(int err){
		
			if(err == 0){
				client.close();				
				printf("Client is offline!\n");
			}else if(err == -1) printf("Fail to receive message from Client!\n");
		}
		client.close();
				
		client.close();
		
	}
	
	server.close();	
	return 0;
}
