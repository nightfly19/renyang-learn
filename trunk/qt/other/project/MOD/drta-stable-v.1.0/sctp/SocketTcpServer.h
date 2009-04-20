

/*
 *  MOD -- Messenger On-the-Drive
 *
 *  Copyright (C) 2006 Kai-Fong Chou - <mod.mapleelpam at gmail.com>
 *
 *  http://mod.0rz.net
 *  http://mod.sf.net
 *
 */
#ifndef _SOCKET_TCP_SEVER_H
#define _SOCKET_TCP_SEVER_H

#include <netinet/in.h>

#include "SocketServer.h"
#include "Socket.h"

class SocketTcpServer : public SocketServer
{
	public:
		SocketTcpServer(int port);

	private:
}; 

#endif
