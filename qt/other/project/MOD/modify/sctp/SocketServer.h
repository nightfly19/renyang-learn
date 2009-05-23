

/*
 *  MOD -- Messenger On-the-Drive
 *
 *  Copyright (C) 2006 Kai-Fong Chou - <mod.mapleelpam at gmail.com>
 *
 *  http://mod.0rz.net
 *  http://mod.sf.net
 *
 */
#ifndef _SOCKET_SEVER_H
#define _SOCKET_SEVER_H

#include <netinet/in.h>
#include <qserversocket.h>
#include <qsocketdevice.h>
#include <qserversocket.h>
#include <qsocketnotifier.h>

#include "Socket.h"
#include "SocketAcceptThread.h"

#undef __SocketServer_Use_Notifier__

//class SocketServer : public QServerSocket , public Socket

#ifndef __SocketServer_Use_Notifier__
class SocketServer : public QServerSocket 
#else
//class SocketServer : public QObject
//moc 不支援這樣的語法 所以還是要手動unmark上面這一行
#endif
{
	Q_OBJECT 
		
	public:
		SocketServer(int port);
		SocketServer(QObject* parent , int type , int protocol , int port);

	public: /* 基面的 Socket Function */
		int Accept(); // block version
		void initAcceptThread();

		int getClientFd(){	return _clnt_socket;	};	
		// renyang - 取得server端代表server的socket
		int getSocketFd(){	return _socket;	};	

		int Bind_Listen();

		/* 提供介面 去access client , 未來應該不要提供 */
		int Send(mPacket& p,int sock = -1);
		int Recv(mPacket& p,int sock = -1);
		void Close();
		void CloseClient();
		void ShutdownClient();

	protected: 
		// 不讓一般User使用 Bind or Listen -> 而使用 Bind_Listen 來設定QSocket 以產生對映的slots與signals
		int Bind();
		int Listen();


	public slots:
		virtual void newConnection(int sock);
		void newConnection2(int sock);
		void readFromClient(int sock);

		virtual void discardFromClient();
	signals:
		void sigNewConnection(int sock);
		void sigReadFromClient(int sock);


	private:
		int listenPort;
		struct sockaddr_in addr;

		SocketAcceptThread *_saThread; // for accept fucntion
		
	protected: /* 從Socket那裡拿來的 private vlue 及 data */
		// renyang - 表示server本身等待某一個client連接的socket
		int _socket;
		// renyang - 在server端表示client端的socket
		int _clnt_socket;
#ifdef __SocketServer_Use_Notifier__
		QSocketNotifier *_notifier;
#endif
	private:
		void initSocket(int family, int type ,int protocol);
}; 

#endif
