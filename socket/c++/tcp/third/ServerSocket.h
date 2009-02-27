
#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include "Socket.h"

class ServerSocket:private Socket
{
	public:
		ServerSocket(int port,Socket &new_connfd,int family=AF_INET,int type=SOCK_STREAM,int protocol=0,int maxconnections=5);
		const ServerSocket& operator << (const char *s) const;
};

#endif
