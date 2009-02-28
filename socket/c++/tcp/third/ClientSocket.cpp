
#include "ClientSocket.h"

ClientSocket::ClientSocket(char *host,int port,int family,int type,int protocol):Socket()
{
	// 建立socket file descripter
	if (!Socket::create(family,type,protocol))
	{
		throw SocketException ( "Could not create client socket." );
	}

	// 連線到server
	if (!Socket::connect (host,port,family))
	{
		throw SocketException ( "Could not bind to port." );
	}
}

// 只是把資料傳送到另一端,所以,不需要回傳值
void ClientSocket::operator << (const char *s) const
{
	if(!Socket::send(s))
	{
                 throw SocketException ( "Could not write to socket." );
        }
}

void ClientSocket::operator >> (char *s) const
{
	if(Socket::recv(s)<=0)
        {
		throw SocketException ( "Could not read from socket." );
        }
}
