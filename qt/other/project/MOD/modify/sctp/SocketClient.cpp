

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
#include <netdb.h>

#include "Error.h"
#include "SocketClient.h"
#include "global.h"


SocketClient::SocketClient()
	: Socket() 
	, _read_notifier(NULL), _excp_notifier(NULL)
{
#ifdef REN_DEBUG
	qWarning(QString("SocketClient::SocketClient()"));
#endif
	init();
}
SocketClient::SocketClient(int protocol)
	: Socket(AF_INET , SOCK_STREAM , protocol) 
	, _read_notifier(NULL), _excp_notifier(NULL)
{
#ifdef REN_DEBUG
	qWarning(QString("SocketClient::SocketClient()"));
#endif
	init();
}
SocketClient::SocketClient(int type , int protocol)
	: Socket(AF_INET , type , protocol) 
	, _read_notifier(NULL), _excp_notifier(NULL)
{
#ifdef REN_DEBUG
	qWarning(QString("SocketClient::SocketClient()"));
#endif
	init();
}

SocketClient::~SocketClient()
{
#ifdef REN_DEBUG
	qWarning(QString("SocketClient::~SocketClient()"));
#endif
//	if(_read_notifier)	delete _read_notifier; 
//	if(_excp_notifier)	delete _excp_notifier; 
}

void SocketClient::init()
{
#ifdef REN_DEBUG
	qWarning(QString("SocketClient::init()"));
#endif
	server_ip = "";
	conn_port = 0;

	//_qsocket = 0;
	connected = false;

	bzero((void *)&serverAddr, sizeof(serverAddr));

}

int SocketClient::Connect(QString ip,int port)
{
#ifdef REN_DEBUG
	qWarning(QString("SocketClient::Connect(%1,%2)").arg(ip).arg(port));
#endif
	int ret=0;
	struct hostent *hostInfo;

	/* initial */
	server_ip = ip;
	conn_port  = port;
	debug("test test one two three");

	/* prepare */
	hostInfo = gethostbyname( server_ip . latin1() );
	if(hostInfo == NULL){
		throw Error("gethostbyname error");
	}
	serverAddr.sin_family = AF_INET;
	memcpy((char *) &serverAddr.sin_addr.s_addr,
			         hostInfo->h_addr_list[0], hostInfo->h_length);
	serverAddr.sin_port = htons(conn_port);

	debug("test test one two three 2");

	/* real connection */ 
	ret = ::connect(_socket , (struct sockaddr *) &serverAddr, sizeof(serverAddr));

	debug("test test one two three 3 ret = %d",ret);

	if(ret < 0 ){ 
		throw Error("connection refuse"); 
	}
	debug("test test one two three 4");

	// 設定socket,必需注意_socket是己connected的狀態
	setSocket(_socket);

	return ret;
}

void SocketClient::setSocket(int s)
{
#ifdef REN_DEBUG
	qWarning(QString("SocketClient::setSocket(%1)").arg(s));
#endif
	_socket = s;

	if(_read_notifier)	delete _read_notifier;

	_read_notifier = new QSocketNotifier( _socket , QSocketNotifier::Read,
			this, "read to read" );
	connect( _read_notifier, SIGNAL(activated(int)),
			this, SLOT(ReadFromServer()) );

	if(_excp_notifier)	delete _excp_notifier;

	_excp_notifier = new QSocketNotifier( _socket , QSocketNotifier::Exception,
			this, "read to read" );
	connect( _excp_notifier, SIGNAL(activated(int)),
			this, SLOT(DiscardFromServer()) );


	/* set connected */
	connected = true;

}

void SocketClient::Close()
{
#ifdef REN_DEBUG
	qWarning(QString("SocketClient::Close()"));
#endif
	Socket::Close();
	delete _read_notifier;
	delete _excp_notifier; 

	connected = false;
}

// 把與server的連線中斷
void SocketClient::DiscardFromServer()
{
#ifdef REN_DEBUG
	qWarning(QString("SocketClient::DiscardFromServer()"));
#endif
	debug("SocketClient::DiscardFromServer()"); 
	emit sigDiscardFromServer();
}

// 由server端讀取資料
void SocketClient::ReadFromServer()
{
#ifdef REN_DEBUG
	qWarning(QString("SocketClient::ReadFromServer()"));
#endif

	debug("SocketClient::ReadFromServer()");
	emit sigReadFromServer();
}

