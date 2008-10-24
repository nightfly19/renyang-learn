
#include "mysource.h"
#include <qpixmap.h>
#include <qdragobject.h>

MySource::MySource(QWidget *parent,const char *name):QLabel(parent,name){
	setPixmap(QPixmap("cater1.png"));
}

void MySource::dragEnterEvent(QDragEnterEvent *){

}

void MySource::dragLeaveEvent(QDragLeaveEvent *){

}

void MySource::mousePressEvent(QMouseEvent *){
	QDragObject *drobj = 0;

	if (pixmap()){
		drobj = new QImageDrag(pixmap()->convertToImage(),this);

		QPixmap pm;

		pm.convertFromImage(pixmap()->convertToImage());
		drobj->setPixmap(pm,QPoint(0,0));
	}
	if (drobj){
		drobj->dragCopy();
	}
}

