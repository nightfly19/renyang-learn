
#ifndef QOPENCVWIDGET_H
#define QOPENCVWIDGET_H

#include <opencv/cv.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qwidget.h>
#include <qimage.h>
#include <qcanvas.h>
#include "MyCanvas.h"

class QOpenCVWidget:public QWidget {
	private:
		QCanvasView *view;
		QVBoxLayout *layout;
		MyCanvas *canvas;
		QCanvasPixmapArray *array;
		QCanvasSprite *sprite;

		QImage image;
	public:
		QOpenCVWidget(QWidget *parent=0);
		~QOpenCVWidget(void);
		void putImage(IplImage*);
};

#endif
