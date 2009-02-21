
#include "MyCanvas.h"

MyCanvas::MyCanvas(int w,int h):QCanvas(w,h) {
	pixmap = new QCanvasPixmap("cater1.png");
	array = new QCanvasPixmapArray();
	array->setImage(0,pixmap);
	sprite = new QCanvasSprite(array,this);
	sprite->show();
}

