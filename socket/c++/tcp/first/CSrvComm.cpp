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
	// 當宣告一個Server物件時, 同時做的動作為socket(),bind(),listen()
	Server server(port);
	// 使用的是一個沒有參數的建構子
	Client client;
	
	while(true){
		// 回傳是int,但是client是Client型別耶,是物件耶??
		// 因為有設定運算子重載
		// server.accept()回傳的值是在server端表示client的file descriptor
		client = server.accept();
		
		try{
			// 由client送出一個訊息??真是神奇,這裡是server竟然可以控制client端??
			// client是在server端代表的client的物件(所以是正確的)
			// 回傳值是送出的大小(bytes)
			ret = client.send("Hello! I'm C++ Server.\n");
			// 丟出整數型別的例外錯誤
			if(ret <= 0 ) throw ret;
		
		}catch(int err){
	
			if(err == 0) {
				client.close();
				printf("Client is offline!\n");
			}else if(err == -1) printf("Fail to send message to Client!\n");
		}
		
		try{
			// 由client接收字元,很神奇耶,傳送端與接收端都由client來做??
			// 沒有錯啦, 是client物件是在server端代表client的物件啦
			ret = client.recv(buf);
			if(ret <= 0) throw ret;
			// 把收到的buffer列印出來
			printf("%s", buf);
				
		}catch(int err){
		
			if(err == 0){
				client.close();				
				printf("Client is offline!\n");
			}else if(err == -1) printf("Fail to receive message from Client!\n");
		}
		client.close();
		
	}
	
	server.close();	
	return 0;
}
