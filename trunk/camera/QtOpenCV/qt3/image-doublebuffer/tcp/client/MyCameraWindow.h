
#ifndef MYCAMERAWINDOW_H
#define MYCAMERAWINDOW_H

#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qimage.h>
#include <qpixmap.h>
#include "imagedata.h"

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
};

#endif
