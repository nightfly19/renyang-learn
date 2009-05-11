#ifndef SCTPCLIENTSOCKET_H
#define SCTPCLIENTSOCKET_H

#include <qsocketnotifier.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>

class SCTPClientSocket:public QObject
{
	Q_OBJECT
	public:
		SCTPClientSocket(QObject *parent=0,const char *name=0);
		void connectToHost(const QString &host, Q_UINT16 port);
		int SCTPSendmsg(const void *,size_t,struct sockaddr *,socklen_t, uint32_t,uint32_t,uint16_t, uint32_t, uint32_t);
		int sendMsg(QString );

	private:
		QSocketNotifier *sctp_notification;
		int connSock;
};

#endif
