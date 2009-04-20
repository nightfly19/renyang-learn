

/*
 *  MOD -- Messenger On-the-Drive
 *
 *  Copyright (C) 2006 Kai-Fong Chou - <mod.mapleelpam at gmail.com>
 *
 *  http://mod.0rz.net
 *  http://mod.sf.net
 *
 */
#ifndef _SOCKET_SCTP_SEVER_H
#define _SOCKET_SCTP_SEVER_H

#include <netinet/in.h>

#include "SocketServer.h"
#include "Socket.h"

class SocketSctpServer : public SocketServer
{
	Q_OBJECT

	public:
		/* TODO: maple .. 加上parent , 以用QT 的mm , */
		SocketSctpServer(QObject* parent,int port);

	public slots:
		virtual void discardFromClient();
		virtual void readFromClient();
		virtual void newConnection(int);


	signals:
		void newConnectSCTP(int);

	public: /* Sctp Extention */
		int SctpSetMaxStream(int num = 5);
		int SctpGetAssocID();
		int SctpEnable();
		/* SCTP I/O (+multi stream) */
		int SctpSendMsg(mPacket& p , int str_num = 0, int sock = -1);
		int SctpRecvMsg(mPacket& p , int& str_num , int sock = -1);

	private:
}; 

#endif
