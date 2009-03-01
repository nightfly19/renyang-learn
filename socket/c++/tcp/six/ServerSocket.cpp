
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
	// 建立connect的socket
	if (!Socket::accept()) {
		throw SocketException ( "Could not accept socket." );
	}
}

// 只是把資料傳送到另一端,所以,不需要回傳值
void ServerSocket::operator << (const char *s) const
{
	if(!Socket::send(s,strlen(s)))
	{
		throw SocketException ( "Could not write to socket." );
	}
}

void ServerSocket::Sendbyte(const char *s,int length)
{
	if(!Socket::send(s,length))
	{
		throw SocketException("Could not write to socket.");
	}
}

void ServerSocket::operator >> (char *s) const
{
	if(Socket::recv(s)<=0)
	{
		throw SocketException ( "Could not read from socket." );
	}
}

void ServerSocket::accept()
{
	if (!Socket::accept())
	{
		throw SocketException ( "Could not accept socket." );
	}
}

bool ServerSocket::close_connfd()
{
	return Socket::close_connfd();
}
