
#ifndef MESSAGE_H
#define MESSAGE_H

#include <qobject.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <qsocketnotifier.h>

class message:public QObject
{
	Q_OBJECT
	public:
		message();

	public slots:
		void printsys();

	private:
		int sock_fd;
		int msg_flags;
		struct sockaddr_in servaddr, cliaddr;
		struct sctp_sndrcvinfo sri;
		struct sctp_event_subscribe evnts;
		socklen_t len;
		size_t rd_sz;
		int ret_value;
		char readbuf[256];
		QSocketNotifier *sn;

};

#endif

