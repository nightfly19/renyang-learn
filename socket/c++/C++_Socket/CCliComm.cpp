#include <stdio.h>
#include <stdlib.h>

#include "Client.h"

int main(int argc, char* argv[])
{
	int port = 2345;
	char ip[40];
	
	switch(argc){
		case 1:
			strcpy(ip, "127.0.0.1");
			port = 2345;
			break;
		case 3:
			strcpy(ip, argv[1]);
			port = atoi(argv[2]);
			break;
		default:
			printf("syntax error: server [ip] [port]\n");
			break;		
	}
	
	int ret;
	char buf[MAX_BUF];
		
	Client client(ip, port);
	
	try{
		// ret = client.recv(buf);
		ret = client >> buf;
		if(ret <= 0) throw ret;
		printf("%s", buf);
	
	}catch(int err){
	
		if(err == 0) printf("Server is offline!\n");
		else if(err == -1) printf("Fail to receive message from Server!\n");
	}	
	
	try{
		//ret = client.send("Hello! I'm C++ Client.\n");
		ret = client << "Hello! I'm C++ Client.\n";			
		if(ret <= 0 ) throw ret;
		
	}catch(int err){
	
		if(err == 0) printf("Server is offline!\n");
		else if(err == -1) printf("Fail to send message to Server!\n");
	}
	
	client.close();
	
	return 0;
}
