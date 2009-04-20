

/*
 *  MOD -- Messenger On-the-Drive
 *
 *  Copyright (C) 2006 Kai-Fong Chou - <mod.mapleelpam at gmail.com>
 *
 *  http://mod.0rz.net
 *  http://mod.sf.net
 *
 */
#ifndef _SOCKET_SCTP_CLIENT_H
#define _SOCKET_SCTP_CLIENT_H

#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/sctp.h>

#include <qstring.h>
#include <qptrlist.h>
#include <qstrlist.h>

#include "mPacket.h"
#include "SocketClient.h"
#include "Socket.h"

class SocketSctpClient: public SocketClient
{
	Q_OBJECT

	public:
		SocketSctpClient(); 
		~SocketSctpClient(); 


	public:
		/* Sctp extention */ 
		int SctpSetMaxStream(int num);
		int SctpGetAssocID();
		int SctpGetState();
		int SctpEnable();
		/* SCTP I/O (+multi stream) */
		int SctpSendMsg(mPacket& p , int str_num = 0 , int sock = -1);
		int SctpRecvMsg(mPacket& p , int& str_num , int sock = -1);

		// just add a record, not real connect
		int SctpAddConn(QString& , int);
		int SctpConnect();
		
	private:
		//QPtrList<sockaddr_in> _connx_ip_list;
		QStrList _connx_ip_list;
		int _port;
}; 

#endif
