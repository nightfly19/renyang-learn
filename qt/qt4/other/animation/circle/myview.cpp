
#include "myview.h"
#include "myscene.h"

MyView::MyView(QWidget *parent):QGraphicsView(parent) {
	setRenderHint(QPainter::Antialiasing, true); // 讓畫面變的比較平滑
}

MyView::MyView(MyScene *scene,QWidget *parent):QGraphicsView(scene,parent) {
	setRenderHint(QPainter::Antialiasing, true); // 讓畫面變的比較平滑
}

