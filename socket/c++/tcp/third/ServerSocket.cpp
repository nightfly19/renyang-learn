
#include "ServerSocket.h"
#include "SocketException.h"

ServerSocket::ServerSocket(int port,Socket &new_connfd,int family,int type,int protocol,int maxconnections):Socket()
{
	if(!Socket::create(family,type,protocol)) {
		throw SocketException ( "Could not create server socket." );
	}
	if(!Socket::bind (port,family)) {
		throw SocketException ( "Could not bind to port." );
	}
	if(!Socket::listen(maxconnections)) {
		throw SocketException ( "Could not listen to socket." );
	}
	// 建立connect的socket
	if (!Socket::accept(new_connfd)) {
		throw SocketException ( "Could not accept socket." );
	}
}

const ServerSocket& ServerSocket::operator << (const char *s) const
{
	if(!Socket::send(s))
	{
		throw SocketException ( "Could not write to socket." );
	}
	// return *this;
}
