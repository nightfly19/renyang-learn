
#include "QOpenCVWidget.h"
#include <qpainter.h>
#include <qpixmap.h>

// Constructor
QOpenCVWidget::QOpenCVWidget(QWidget *parent):QWidget(parent) {
	// do nothing
}

QOpenCVWidget::~QOpenCVWidget(void) {
	// do nothing
}

void QOpenCVWidget::putImage(IplImage *cvimage) {
	// 宣告一個pixmap來當作double-buffer
	QPixmap pix(size());
	QPainter p(&pix);
	int cvIndex,cvLineStart;
	// switch between bit depths
	switch (cvimage->depth) {
		case IPL_DEPTH_8U:
			switch (cvimage->nChannels) {
				case 3:
					if ((cvimage->width != image.width()) || (cvimage->height != image.height())) {
						resize(QSize(cvimage->width,cvimage->height));
					}
					cvIndex = 0; cvLineStart = 0;
					for (int y = 0;y<cvimage->height;y++) {
						unsigned char red,green,blue;
						cvIndex = cvLineStart;
						for (int x = 0;x < cvimage->width;x++) {
							red = cvimage->imageData[cvIndex+2];
							green = cvimage->imageData[cvIndex+1];
							blue = cvimage->imageData[cvIndex+0];

							p.setPen(QColor((int)red,(int)green,(int)blue));
							p.drawPoint(x,y);
							cvIndex+=3;
						}
						cvLineStart += cvimage->widthStep;
					}
					break;
				default:
					printf("This number of channels is not supported\n");
					break;
			}
			break;
		default:
			printf("This type of IplImage is not implemented in QOpenCVWidget\n");
			break;
	}
	p.end();
	p.begin(this);
	p.drawPixmap(0,0,pix);
}
