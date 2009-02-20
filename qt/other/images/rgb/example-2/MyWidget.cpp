
#include "MyWidget.h"
#include <qpainter.h>

MyWidget::MyWidget(QWidget *parent,const char *name):QWidget(parent,name) {
	// do nothing
}

void MyWidget::paintEvent( QPaintEvent * ) {
	// 
	QPainter p(this);
	p.setPen(QColor(255,0,0));
	p.drawPoint(20,20);
}
