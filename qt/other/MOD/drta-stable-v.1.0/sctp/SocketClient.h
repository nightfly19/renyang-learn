

/*
 *  MOD -- Messenger On-the-Drive
 *
 *  Copyright (C) 2006 Kai-Fong Chou - <mod.mapleelpam at gmail.com>
 *
 *  http://mod.0rz.net
 *  http://mod.sf.net
 *
 */
#ifndef _SOCKET_CLIENT_H
#define _SOCKET_CLIENT_H

#include <netinet/in.h>
#include <qstring.h>
#include <qsocket.h>
#include <qsocketnotifier.h>

#include "mPacket.h"
#include "Socket.h"

class SocketClient: public Socket
{
	Q_OBJECT

	public:
		SocketClient();
		SocketClient(int protocol);
		SocketClient(int type , int protocol);
		~SocketClient();

		void setSocket(int); // use with SocketClient()

		int Connect(QString ip,int port);

		virtual void Close();
	private:
		/* real initial function */
		void init();

	private slots:
		void ReadFromServer();
		void DiscardFromServer();
	signals:
		void sigReadFromServer();
		void sigDiscardFromServer();

	private:
		int conn_port;
		QString server_ip;
		struct sockaddr_in serverAddr;

		//QSocket* _qsocket;
		bool connected;

		QSocketNotifier* _read_notifier;
		QSocketNotifier* _excp_notifier;
}; 

#endif
