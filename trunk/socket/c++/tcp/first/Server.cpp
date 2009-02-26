#include "Server.h"

Server::Server(const int port)
{
	state = true;
	
#ifdef _WIN32
	initWsa();
#endif
	
	create();
	bind(port);
	listen();
}

Server::~Server()
{
	if(socket != INVALID_SOCKET){
		
	#ifdef _WIN32
		closesocket(socket);
	#else
		::close(socket);
	#endif
	
	}
	
#ifdef _WIN32
	WSACleanup();
#endif

}

#ifdef _WIN32
bool Server::initWsa()
{
	int WSAState;
	WSADATA wsaData;
	
	WSAState = WSAStartup(0x101, &wsaData);
	if(WSAState){
		printf("Initial WSA Error!, error code:%d\n", WSAState);
		state = false;
		
		return false;
	}
	
	return true;
}
#endif

bool Server::create()
{
	if( ! isValid() ) return false;
	
	// 後面的::是表示是外部的函數
	socket = ::socket(PF_INET,SOCK_STREAM,0);
	if(socket == INVALID_SOCKET){
		perror("Error occurred in socket()");
		state = false;		
		
		return false;
	}
	
	return true;
}

bool Server::bind(const int port)
{
	if( ! isValid() ) return false;
	
	struct sockaddr_in local;
	
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	local.sin_port = htons((u_short)port);
	
	if(::bind(socket, (struct sockaddr*)&local, sizeof(local)) == SOCKET_ERROR){
		perror("Error occurred in bind()");
	
		this->close();
		
		state = false;
		return false;
	}
	return true;
}

bool Server::listen()
{
	if( ! isValid() ) return false;
	
	if(::listen(socket, 10) == SOCKET_ERROR){
		perror("Error occurred in listen()");
		
		this->close();
		
		state = false;		
		
		return false;
	}
	return true;
}

SOCKET Server::accept()
{
	if( ! isValid() ) return false;
	
	struct sockaddr_in sa_client;
	
#ifdef _WIN32
	int addr_len = sizeof(sa_client); 
#else	
	socklen_t addr_len = sizeof(sa_client);
#endif
	
  	memset(&sa_client,'\0',sizeof(struct sockaddr_in));
	
  	printf("#Waiting for client to connect...\n");
	
	SOCKET ClientSocket;	
	
	ClientSocket = ::accept( socket, (struct sockaddr *)&sa_client, &addr_len );
	
	if (ClientSocket == INVALID_SOCKET){
		perror("Error occurred in accept()");
		return INVALID_SOCKET;
	}else
		// inet_ntoa把ip轉成數字與.的字串
		printf("Socket(%d) Connection from %s:%d\n",ClientSocket ,inet_ntoa(sa_client.sin_addr), sa_client.sin_port);
	
	return ClientSocket;
}

bool Server::isValid() const
{
	if(state) return true;
	else return false;	
}

void Server::close()
{
	if(socket != INVALID_SOCKET){
	
	#ifdef _WIN32
		closesocket(socket);
	#else
		::close(socket);
	#endif
		socket = INVALID_SOCKET;
	}
}

