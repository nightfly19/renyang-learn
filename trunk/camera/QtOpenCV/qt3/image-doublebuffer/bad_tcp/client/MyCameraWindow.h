
#ifndef MYCAMERAWINDOW_H
#define MYCAMERAWINDOW_H

#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qimage.h>
#include <qpixmap.h>
#include "imagedata.h"
#include "ClientSocket.h"
#include "ServerSocket.h"
#include "transmit.h"
#include <qsocketnotifier.h>

class MyCameraWindow:public QWidget
{
	Q_OBJECT
	public:
		MyCameraWindow(QWidget *parent=0);
		void setCameraImage(struct imagedata&);
		~MyCameraWindow();
	
	private:
		QLabel *imagelabel;
		QVBoxLayout *layout;
		QImage image;
		QPixmap pix;
		QSocketNotifier *sn;
		ClientSocket *client_socket;
		struct imagedata pixel;
		int begin;

	public slots:
		void Recvdata();
};

#endif
