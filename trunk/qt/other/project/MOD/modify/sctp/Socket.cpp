

/*
 *  MOD -- Messenger On-the-Drive
 *
 *  Copyright (C) 2006 Kai-Fong Chou - <mod.mapleelpam at gmail.com>
 *
 *  http://mod.0rz.net
 *  http://mod.sf.net
 *
 */

/* TODO:
 *
 * Notifier 還沒搞定 
 *
 */

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>


#include <string.h>

#include "Socket.h"
#include "Error.h"
#include "mPacket.h"

Socket::Socket()
{
#ifdef REN_DEBUG
	qWarning(QString("Socket::Socket()"));
#endif
	// TCP default
	//initSocket(AF_INET , SOCK_STREAM , 0);
	// !! INITIAL NOTHING

}

Socket::Socket(int family = AF_INET , int type = SOCK_STREAM , int protocol = 0)
{
#ifdef REN_DEBUG
	qWarning(QString("Socket::Socket()"));
#endif
	initSocket(family , type , protocol);
}

Socket::~Socket()
{
#ifdef REN_DEBUG
	qWarning(QString("Socket::~Socket()"));
#endif

}
/* private function */
void Socket::initSocket(int family = AF_INET , int type = SOCK_STREAM , int protocol = 0)
{
#ifdef REN_DEBUG
	qWarning(QString("Socket::initSocket()"));
#endif
	_socket = ::socket( family , type , protocol );

	if( _socket< 0 )
		throw Error("Could not create socket");

}

int Socket::Bind(int port)
{
#ifdef REN_DEBUG
	qWarning(QString("Socket::Bind(%1)").arg(port));
#endif
	int ret = 0;
	struct sockaddr_in servaddr;

	bzero( (void *)&servaddr, sizeof(servaddr) );
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl( INADDR_ANY );
	servaddr.sin_port = htons(port);

	ret = bind(_socket,(struct sockaddr *)&servaddr, sizeof(servaddr));
	if( ret )
		throw Error("Could not bind port ");

	debug("bind ok");

	return ret;
}

int Socket::Send(mPacket& p,int sock)
{
#ifdef REN_DEBUG
	qWarning(QString("Socket::Send()"));
#endif
	int ret=0;

	if(sock  == -1) sock=_socket;
			
	debug("Socket::Send sock = %d",sock); 
	ret = send(sock, p.getBufPtr() , p.Size() , 0);
	if(ret < 0)
		throw Error("Socket::send error");
	return ret;
}

int Socket::Recv(mPacket& p , int sock)
{
#ifdef REN_DEBUG
	qWarning(QString("Socket::Recv()"));
#endif
	int ret = 0;

	if(sock  == -1) sock=_socket;
	debug("Socket::Recv sock = %d",sock); 

	ret = recv(sock, p.getBufPtr() , p.MaxSize() , 0);
	p . setSize( ret > 0 ? ret : ret );

	debug("Socket::Recv: ret = %d",ret);
	if(ret  < 0)
		return ret;
	p . setSize(ret);

	return ret;
}

void Socket::Close()
{
#ifdef REN_DEBUG
	qWarning(QString("Socket::Close()"));
#endif
	::close(_socket); 
	_socket = 0;

}
void Socket::Shutdown()
{
#ifdef REN_DEBUG
	qWarning(QString("Socket::Shutdown()"));
#endif
	::shutdown(_socket , 2); 

}
