
#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include "Socket.h"
#include "SocketException.h"

class ClientSocket:private Socket
{
	public:
		ClientSocket (char *host,int port,int family=AF_INET,int type=SOCK_STREAM,int protocol=0);
		void operator << (const char *s) const;
		int operator >> (char *s) const;
};

#endif

