
#include "myview.h"
#include "myscene.h"

MyView::MyView(QWidget *parent):QGraphicsView(parent) {
	// do nothing
}

MyView::MyView(MyScene *scene,QWidget *parent):QGraphicsView(scene,parent) {
	// do nothing
}

