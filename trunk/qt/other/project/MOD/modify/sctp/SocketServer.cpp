

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


// 若產生物件時,只有給port值的話,那預設就是Tcp傳輸
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

// 建立socketserver物件時,有另外宣告type與protocol
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
// 預設值為tcp傳輸
void SocketServer::initSocket(int family = AF_INET , int type = SOCK_STREAM , int protocol = 0)
{
	// 呼叫c語言中的socket函數
	// int socket(int domain, int type , int protocol);
	// 回傳為socket的file descripter
	_socket = ::socket( family , type , protocol );

	// 若失敗回傳值為-1
	if( _socket< 0 )
		throw Error("Could not create socket");

}


// 等待client端connect
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

// 我猜是想要建立thread,可以多個client端連線吧??
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

// renyang - 把socket與address_in連結在一起
int SocketServer::Bind()
{ 
	int ret = 0;
	struct sockaddr_in servaddr;

	bzero( (void *)&servaddr, sizeof(servaddr) );
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl( INADDR_ANY );
	servaddr.sin_port = htons(listenPort);

	// renyang - 使用c語言中的bind函式:int bind(int sid,struct sockaddr *,int len)
	ret = ::bind(_socket,(struct sockaddr *)&servaddr, sizeof(servaddr));
	if( ret )
		throw Error("Could not bind port ");

	debug("bind ok");

	return ret;
}
// 宣告最大可以的連線數
int SocketServer::Listen()
{ 
	// 表示目前最大連線數為1
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
	// renyang - 此_socket已為connected的狀態
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

	// 建立一個警告物件,特別去注意read事件
	QSocketNotifier *_clnt_notifier = new QSocketNotifier( sock , QSocketNotifier::Read,
			this, "accepting new connections" );

	connect( _clnt_notifier, SIGNAL(activated(int)), this, SLOT(readFromClient(int)) );

	_clnt_socket = sock;

	// talk to outside ui/non-ui
	// 告知別人有新的連線啦
	emit sigNewConnection(sock);
}

void SocketServer::discardFromClient()
{
	::close(_clnt_socket);
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

// 由server傳送資料到已連線的connect_fd
int SocketServer::Send(mPacket& p,int sock)
{
	int ret=0;

	if(sock  == -1) sock=_socket;
			
	// 傳送封包出去,準備傳送的封包值為p.Size(),回傳值ret為傳出去封包的大小,若為-1表示傳送失敗
	// p.Size()的部分還不太懂
	ret = ::send(sock, p.getBufPtr() , p.Size() , 0);
	if(ret < 0)
		throw Error("SocketServer::send error");
	return ret;
}

// 由client端接收封包
int SocketServer::Recv(mPacket& p , int sock)
{
	int ret = 0;

	if(sock  == -1) sock=_socket;


	// 用來接收封包, 把收到的封包放到p.getBufPtr()的位址上
	ret = recv(sock, p.getBufPtr() , p.MaxSize() , 0);
	// ret若是-1則表示失敗
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

// 把connect_fd關掉
void SocketServer::CloseClient()
{
	if(_clnt_socket)
	       	::close(_clnt_socket);

	_clnt_socket = 0;

}
