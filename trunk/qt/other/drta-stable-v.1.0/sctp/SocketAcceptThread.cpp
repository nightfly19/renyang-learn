

/*
 *  MOD -- Messenger On-the-Drive
 *
 *  Copyright (C) 2006 Kai-Fong Chou - <mod.mapleelpam at gmail.com>
 *
 *  http://mod.0rz.net
 *  http://mod.sf.net
 *
 */

#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <netinet/in.h>
#include "SocketAcceptThread.h"

SocketAcceptThread::SocketAcceptThread(int socket)
	: _socket(socket)
{
}


void SocketAcceptThread::run()
{
	// Blocking 

	struct sockaddr_in addr;
	int size = sizeof(addr);

	while(1)
	{
		debug("Thread : waitting new connection");
		int connSock = ::accept( _socket, (struct sockaddr *)&addr, (socklen_t*) &size);
		debug("Thread : new connection");

		if(connSock < 0 ){
			throw Error("accept error");

		}
		emit newConnection( connSock );
	}
}

