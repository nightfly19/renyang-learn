

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
#include "global.h"

#include "SocketSctpServer.h"
#include "SctpSocketHandler.h"

SocketSctpServer::SocketSctpServer(QObject* parent ,int port)
	: SocketServer(parent , SOCK_STREAM , IPPROTO_SCTP , port)
{
	debug("有設 IPPROTO_SCTP");
	SctpEnable(); 
}
void SocketSctpServer::discardFromClient()
{
	debug("SocketSctpServer::discardFromClient()");
}

void SocketSctpServer::readFromClient()
{
	debug("SocketSctpServer::readFromClient()");
}

int SocketSctpServer::SctpSetMaxStream(int num)
{
/*
	int ret; 
	struct sctp_initmsg initmsg;

	memset( &initmsg, 0, sizeof(initmsg) );
	initmsg.sinit_num_ostreams = num; 
	initmsg.sinit_max_instreams = num;
	initmsg.sinit_max_attempts = num - 1;
	ret = setsockopt( _socket, IPPROTO_SCTP, SCTP_INITMSG,
			&initmsg, sizeof(initmsg) );
	return ret;*/
	return SctpSocketHandler::SctpSetMaxStream( _socket , num );
}

int SocketSctpServer::SctpGetAssocID()
{
	return SctpSocketHandler::SctpGetAssocID(_socket);
}

int SocketSctpServer::SctpEnable()
{
/*
	 struct sctp_event_subscribe events;
	int ret;

	memset( (void *)&events, 0, sizeof(events) );
	events.sctp_data_io_event = 1;
	ret = setsockopt( _socket , SOL_SCTP, SCTP_EVENTS,
			(const void *)&events, sizeof(events) );

	return ret;
	*/
	return SctpSocketHandler::SctpEnable(_socket);
}
int SocketSctpServer::SctpSendMsg(mPacket& p,int str_num, int sock)
{
	int ret=0;

	if(sock  == -1) sock=_socket;
			
//	ret = send(sock, p.getBufPtr() , p.Size() , 0);
	ret = sctp_sendmsg(sock, p.getBufPtr() , p.Size() 
			, NULL , 0 , 0 , 0 , str_num , 0 , 0);
	if(ret < 0)
		throw Error("Socket::send error");
	return ret;
}


int SocketSctpServer::SctpRecvMsg(mPacket& p,int& str_num, int sock)
{
	int ret=0;
	struct sctp_sndrcvinfo sndrcvinfo;

	if(sock  == -1) sock=_socket;
			
	ret = sctp_recvmsg(sock, p.getBufPtr() , p.MaxSize() 
			, NULL , 0 , &sndrcvinfo , 0);

	if(ret < 0)
		throw Error("Socket::send error");

	str_num = sndrcvinfo.sinfo_stream;
	return ret;
}


void SocketSctpServer::newConnection(int sock)// 在QSocketServer傳過來
{
	debug("SocketSctpServer : newConnection(%d)" , sock);

	emit newConnectSCTP(sock);
}

