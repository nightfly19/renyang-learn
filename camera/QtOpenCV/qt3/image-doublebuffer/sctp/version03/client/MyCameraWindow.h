#ifndef MYCAMERAWINDOW_H
#define MYCAMERAWINDOW_H

#include <qlayout.h>
#include <qlabel.h>
#include <qimage.h>
#include <qstring.h>
#include <qsocketnotifier.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>

#include "common.h"

class MyCameraWindow:public QWidget
{
	Q_OBJECT
	public:
		MyCameraWindow(QWidget *parent=0);
		bool Connect2Host(QString, int);
		void startVideo();
		void start();

	private:
		int connfd;
		struct sockaddr_in servaddr;
		QBoxLayout *layout;
		QLabel *imagelabel;
		QSocketNotifier *notifier;
		struct image_matrix matrix;
		int begin;
		QImage image;
		void setCameraImage();
		bool receiving;

	public slots:
		void Recvdata();
	
	protected:
		void timerEvent(QTimerEvent*);
};

#endif

