
#include "QOpenCVWidget.h"
#include <qpainter.h>
#include <qpixmap.h>

// Constructor
QOpenCVWidget::QOpenCVWidget(QWidget *parent):QWidget(parent) {
	layout = new QVBoxLayout(this);
	view = new QCanvasView(0,this);
	canvas = new MyCanvas(500,400);
	view->setCanvas(canvas);
	layout->addWidget(view);
	array = new QCanvasPixmapArray();
}

QOpenCVWidget::~QOpenCVWidget(void) {
	// do nothing
}

void QOpenCVWidget::putImage(IplImage *cvimage) {
	int cvIndex,cvLineStart;
	// switch between bit depths
	switch (cvimage->depth) {
		case IPL_DEPTH_8U:
			switch (cvimage->nChannels) {
				case 3:
					if ((cvimage->width != image.width()) || (cvimage->height != image.height())) {
						QImage temp(cvimage->width,cvimage->height,32);
						image = temp;
						canvas->resize(cvimage->width,cvimage->height);
					}
					cvIndex = 0; cvLineStart = 0;
					for (int y = 0;y<cvimage->height;y++) {
						unsigned char red,green,blue;
						cvIndex = cvLineStart;
						for (int x = 0;x < cvimage->width;x++) {
							red = cvimage->imageData[cvIndex+2];
							green = cvimage->imageData[cvIndex+1];
							blue = cvimage->imageData[cvIndex+0];

							image.setPixel(x,y,qRgb(red,green,blue));
							cvIndex+=3;
						}
						cvLineStart += cvimage->widthStep;
					}
					break;
				default:
					printf("This number of channels is not supported\n");
					break;
			}
			// 把image顯示在canvas上
			array->setImage(0,new QCanvasPixmap(image));
			sprite = new QCanvasSprite(array,canvas);
			sprite->show();
			break;
		default:
			printf("This type of IplImage is not implemented in QOpenCVWidget\n");
			break;
	}
}
