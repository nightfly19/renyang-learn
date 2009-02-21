
#include "MyView.h"

MyView::MyView(QWidget *parent,const char *name):QCanvasView(parent,name) {
	// do nothing
}

MyView::MyView(QCanvas *canvas,QWidget *parent,const char *name):QCanvasView(canvas,parent,name) {
	// do nothing
}
