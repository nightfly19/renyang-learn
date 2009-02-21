
#ifndef MYCANVAS_H
#define MYCANVAS_H

#include <qcanvas.h>

class MyCanvas:public QCanvas {
	public:
		MyCanvas(int w=400,int h=300);
	private:
		QCanvasPixmap *pixmap;
		QCanvasSprite *sprite;
		QCanvasPixmapArray *array;
};

#endif
