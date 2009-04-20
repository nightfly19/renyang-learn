

/*
 *  MOD -- Messenger On-the-Drive
 *
 *  Copyright (C) 2006 Kai-Fong Chou - <mod.mapleelpam at gmail.com>
 *
 *  http://mod.0rz.net
 *  http://mod.sf.net
 *
 */
#ifndef _SOCKET_H
#define _SOCKET_H

#include <qobject.h>
#include <qsocket.h>

#include "mPacket.h"


class Socket  : public QObject
{
	Q_OBJECT

	public:
		Socket();
		Socket(int family, int type ,int protocol);
		~Socket();
		int Bind(int port); 

		

		/* General/Basic I/O */
		virtual int Send(mPacket& p,int sock = -1);
		virtual int Recv(mPacket& p,int sock = -1);
		virtual void Close();
		virtual void Shutdown();

	protected:
		int _socket;
	private:
		/* real initial function */
		void initSocket(int family, int type ,int protocol);

};

#endif
