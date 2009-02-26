#ifndef _SERVER_H_
#define _SERVER_H_

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

class Server{
	public:
		Server(const int port);
		~Server();
	#ifdef _WIN32
		bool initWsa();
	#endif
	
		bool create();
		bool bind(const int port);
		bool listen();
		SOCKET accept();

		bool isValid() const;
		
		void close();			
	private:
		int state;	
		SOCKET socket;		
};

#endif
