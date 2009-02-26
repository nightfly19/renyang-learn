#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#ifdef _WIN32
	#include <winsock.h>
#else
	#define SOCKET int
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	
	#include <netdb.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <unistd.h>
#endif

#define MAX_BUF 1024

class Client{
	public:
		Client(){};
		Client(const char* ip, const int port);
		~Client();
		
	#ifdef _WIN32		
		bool initWsa();
	#endif		
		
		bool create();
		bool connect(const char* ip, const int port);
		
		int send(const char* buf) const;
		int recv(char* buf);
		
		bool isValid() const;
		
		int operator = (SOCKET socket);
		int operator << (const char* buf) const;
		int operator >> (char* buf);
		
		void close();
		
	private:
		int state;
		SOCKET socket;		
};

#endif

