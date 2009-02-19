
#ifndef QOPENCVWIDGET_H
#define QOPENCVWIDGET_H

#include <opencv/cv.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qwidget.h>
#include <qimage.h>

class QOpenCVWidget:public QWidget {
	private:
		QLabel *imagelabel;
		QVBoxLayout *layout;

		QImage image;
	public:
		QOpenCVWidget(QWidget *parent=0);
		~QOpenCVWidget(void);
		void putImage(IplImage*);
};

#endif
