

/*
 *  MOD -- Messenger On-the-Drive
 *
 *  Copyright (C) 2006 Kai-Fong Chou - <mod.mapleelpam at gmail.com>
 *
 *  http://mod.0rz.net
 *  http://mod.sf.net
 *
 */
#ifndef _SOCKET_ACCEPT_THREAD__
#define _SOCKET_ACCEPT_THREAD__

#include <qthread.h>
#include <qvariant.h>
#include <qobject.h>
#include "Error.h"
#include "global.h"


//class SocketAcceptThread : public QObject , public QThread 
class SocketAcceptThread : public QObject
//class SocketAcceptThread  
{
	Q_OBJECT
	public:
		SocketAcceptThread(int socket);

		void setSocket(int s) {	_socket = s;	};

	public: // Inherits From QThread , Real threading function
		void run();
	signals:
		void newConnection(int s);

	private:
		int _socket;


}; 


#endif

