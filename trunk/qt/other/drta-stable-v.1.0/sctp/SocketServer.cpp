

/*
 *  MOD -- Messenger On-the-Drive
 *
 *  Copyright (C) 2006 Kai-Fong Chou - <mod.mapleelpam at gmail.com>
 *
 *  http://mod.0rz.net
 *  http://mod.sf.net
 *
 */

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>

#include <qsocket.h>

#include "Error.h"
#include "SocketServer.h"
#include "global.h"


SocketServer::SocketServer(int port)
	//: QServerSocket(this),listenPort(port),_qsd(NULL)
#ifndef __SocketServer_Use_Notifier__
	: QServerSocket(this),listenPort(port),_saThread(NULL)
#else
	:listenPort(port),_saThread(NULL),_notifier(NULL)
#endif
{
	_clnt_socket = 0;
	initSocket(AF_INET , SOCK_STREAM , 0);
}

SocketServer::SocketServer(QObject* parent,int type , int protocol ,int port)
#ifndef __SocketServer_Use_Notifier__
	:QServerSocket(parent),listenPort(port),_saThread(NULL)
#else
	:listenPort(port),_saThread(NULL),_notifier(NULL)
#endif
{
	debug("SocketServer @@");
	_clnt_socket = 0;
	initSocket(AF_INET , type , protocol);
}


/* private function */
void SocketServer::initSocket(int family = AF_INET , int type = SOCK_STREAM , int protocol = 0)
{
	_socket = ::socket( family , type , protocol );

	if( _socket< 0 )
		throw Error("Could not create socket");

}


int SocketServer::Accept()
{
	// Block accept @@"
	int size = sizeof(addr);
	debug("SocketServer::Accept : block accept");
	int connSock = ::accept( _socket, (struct sockaddr *)&addr, (socklen_t*) &size);
	debug("SocketServer::Accept : end accept");

	if(connSock < 0 ){
		throw Error("accept error");
	}

	return connSock;


}

void SocketServer::initAcceptThread()
{

/*
	// enable the thread to exuction accept 
	if(_saThread)	{
		delete _saThread;
	}
	_saThread = new SocketAcceptThread(_socket);
	_saThread -> start();

	connect( _saThread , SIGNAL(newConnection(int)),
			this, SLOT(newConnection2(int)) );
			*/

	return ;
}

int SocketServer::Bind()
{ 
	int ret = 0;
	struct sockaddr_in servaddr;

	bzero( (void *)&servaddr, sizeof(servaddr) );
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl( INADDR_ANY );
	servaddr.sin_port = htons(listenPort);

	ret = bind(_socket,(struct sockaddr *)&servaddr, sizeof(servaddr));
	if( ret )
		throw Error("Could not bind port ");

	debug("bind ok");

	return ret;
}
int SocketServer::Listen()
{ 
	::listen(_socket, 1);
	return 0;
}

int SocketServer::Bind_Listen()
{
	Bind();
	Listen();

#ifdef __SocketServer_Use_Notifier__
	_notifier = new QSocketNotifier( _socket , QSocketNotifier::Read,
			this, "accepting new connections" );
	connect( _notifier, SIGNAL(activated(int)),
			this, SLOT(newConnection(int)) );
#endif
	QServerSocket::setSocket( _socket );

	return 0;
}

void SocketServer::newConnection(int sock)
{
	debug("SocketServer::newConnection(%d)",sock);

	/*
	QSocket* client = new QSocket( this );
	connect( client , SIGNAL(readyRead()), this, SLOT(readFromClient()) );
	connect( client , SIGNAL(delayedCloseFinished()), this, SLOT(discardFromClient()) );
	client -> setSocket( sock);
	*/

	QSocketNotifier *_clnt_notifier = new QSocketNotifier( sock , QSocketNotifier::Read,
			this, "accepting new connections" );

	connect( _clnt_notifier, SIGNAL(activated(int)), this, SLOT(readFromClient(int)) );

	_clnt_socket = sock;

	// talk to outside ui/non-ui
	emit sigNewConnection(sock);
}

void SocketServer::discardFromClient()
{
	close(_clnt_socket);
}
void SocketServer::readFromClient(int sock)
{
	debug("SocketServer::readFromClient(%d)",sock);
	emit sigReadFromClient(sock);
}

void SocketServer::newConnection2(int sock)
{
	debug("SocketServer::newConnection2(%d)",sock);
	emit sigReadFromClient(sock);
}

int SocketServer::Send(mPacket& p,int sock)
{
	int ret=0;

	if(sock  == -1) sock=_socket;
			
	ret = send(sock, p.getBufPtr() , p.Size() , 0);
	if(ret < 0)
		throw Error("SocketServer::send error");
	return ret;
}

int SocketServer::Recv(mPacket& p , int sock)
{
	int ret = 0;

	if(sock  == -1) sock=_socket;


	ret = recv(sock, p.getBufPtr() , p.MaxSize() , 0);
	if(ret  < 0)
		throw Error("SocketServer::Recv - recv error");
	p . setSize(ret);

	return ret;
}


void SocketServer::Close()
{
	CloseClient();

	if(_socket)
	       	::close(_socket);

	_socket = 0;

}

void SocketServer::ShutdownClient()
{
	if(_clnt_socket)
	       	::shutdown(_clnt_socket, 2);

}

void SocketServer::CloseClient()
{
	if(_clnt_socket)
	       	::close(_clnt_socket);

	_clnt_socket = 0;

}
