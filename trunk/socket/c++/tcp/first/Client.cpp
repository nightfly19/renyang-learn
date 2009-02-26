#include "Client.h"

Client::Client(const char* ip, const int port)
{
	state = true;
	
#ifdef _WIN32
	initWsa();
#endif
	
	create();
	connect(ip, port);
}

Client::~Client()
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
bool Client::initWsa()
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

bool Client::create()
{
	if( ! isValid() ) return false;

	socket = ::socket(PF_INET,SOCK_STREAM,0);
	if(socket == INVALID_SOCKET){
		perror("Error occurred in socket()");
			
		state = false;
		return false;
	}
	return true;
}

bool Client::connect(const char* ip, const int port)
{
	if( ! isValid() ) return false;
	
	struct hostent *h;
	unsigned int addr;
		
	if(isalpha(ip[0])){
		
		/*** get IP by hostname ***/
		h = gethostbyname(ip);
		if(h == NULL){
			printf("Unknown host!\n");
			return false;
		}
		
		addr=*((unsigned long *)h->h_addr);
		
	}else{
		
		addr=inet_addr(ip);
	}
	
	struct sockaddr_in server;
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = addr;
	server.sin_port = htons((u_short)port);
	
	/*** Connect to Server ***/
	if(::connect(socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR){
		perror("Error occurred in connect()");
		this->close();
		return false;
	}else printf("#Socket(%d): Connecting to Server...!\n", socket);
	
	return true;
}

bool Client::isValid() const
{
	if(state) return true;
	else return false;	
}

int Client::operator = (SOCKET socket)
{
	this->socket = socket;
	return socket;		
}


int Client::send(const char* buf) const 
{
	int ret;
	ret = ::send(socket, buf, strlen(buf), 0);
	return ret;
}

int Client::recv(char* buf)
{
	int ret;
	memset(buf, '\0', MAX_BUF);
	ret = ::recv(socket, buf, MAX_BUF, 0);
	return ret;
}

int Client::operator << (const char* buf) const
{
	int ret;
	ret = ::send(socket, buf, strlen(buf), 0);
	return ret;	
}

int Client::operator >> (char* buf)
{
	int ret;
	memset(buf, '\0', MAX_BUF);
	ret = ::recv(socket, buf, MAX_BUF, 0);
	return ret;
}

void Client::close()
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

