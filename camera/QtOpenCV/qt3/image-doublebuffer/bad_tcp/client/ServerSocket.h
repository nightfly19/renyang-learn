
#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include "Socket.h"

class ServerSocket:private Socket
{
	public:
		ServerSocket(int port,int family=AF_INET,int type=SOCK_STREAM,int protocol=0,int maxconnections=5);
		void operator << (const char *s) const;
		void operator >> (char *s) const;
		void accept();
		bool close_connfd();
		void Sendbyte(const char *s,int lenght);
		int get_connfd() { return Socket::get_connfd(); }
};

#endif
