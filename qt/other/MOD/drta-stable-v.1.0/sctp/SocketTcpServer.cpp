

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

#include "Error.h"
#include "SocketTcpServer.h"
#include "global.h"


SocketTcpServer::SocketTcpServer(int port)
	: SocketServer(this,SOCK_STREAM , IPPROTO_TCP , port)
{

}
