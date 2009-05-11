#ifndef SCTPSERVERSOCKET_H
#define SCTPSERVERSOCKET_H

#include <qsocketnotifier.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>

class SCTPServerSocket:public QObject
{
	Q_OBJECT
	public:
		SCTPServerSocket();
		SCTPServerSocket(Q_UINT16, int backlog=1,QObject *parent=0,const char *name=0);
	private:
		QSocketNotifier *sctp_newconnection_notification;
		QSocketNotifier *sctp_notification;
		int listenfd;
		int connfd;
		struct sockaddr_in serv_addr;
		struct sctp_initmsg initmsg;
		int sendMsg(QString);

	signals:
		void newConnection(int);

	public slots:
		int SCTPServerAccept(int);
		int recvMsg(int);
};

#endif
