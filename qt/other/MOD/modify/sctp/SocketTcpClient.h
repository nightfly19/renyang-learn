

/*
 *  MOD -- Messenger On-the-Drive
 *
 *  Copyright (C) 2006 Kai-Fong Chou - <mod.mapleelpam at gmail.com>
 *
 *  http://mod.0rz.net
 *  http://mod.sf.net
 *
 */
#ifndef _SOCKET_TCP_CLIENT_H
#define _SOCKET_TCP_CLIENT_H

#include <netinet/in.h>
#include <qstring.h>

#include "mPacket.h"
#include "SocketClient.h"
#include "Socket.h"

class SocketTcpClient: public SocketClient
{
	public:
		SocketTcpClient(); 
	private:
}; 

#endif
