
#include "ServerSocket.h"
#include "SocketException.h"

ServerSocket::ServerSocket(int port,int family,int type,int protocol,int maxconnections):Socket()
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
}
