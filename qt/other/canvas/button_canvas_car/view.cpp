
#include <qfont.h>
#include <qimage.h>
#include <qdragobject.h>

#include "view.h"

#ifdef MYDEBUG

#include <iostream>
using namespace std;

#endif

QCanvasPixmapArray View::ani; // static要在外面初始化,才會被分配記憶體
QCanvasPixmapArray View::ani_x; // static要在外面初始化,才會被分配記憶體

View::View(QCanvas &canvas,QWidget *parent):QCanvasView(&canvas,parent)
{
	canvas.setBackgroundPixmap(QPixmap("logo.jpg"));
	canvas.resize(1370,1300);
	setFixedSize(sizeHint());
	ani.setImage(0,new QCanvasPixmap("cater1.png"));
	ani_x.setImage(0,new QCanvasPixmap("cater2.png"));
	setAcceptDrops(true); // 可接收Drop
	CanPress = false;
}

void View::dragEnterEvent(QDragEnterEvent *e){
	if(QImageDrag::canDecode(e)){
		e->accept();
	}
}

void View::dropEvent(QDropEvent *e){
	QCanvasSprite *car1 = new QCanvasSprite(&ani,canvas());
	car1->show();
	car1->move(e->pos().x(),e->pos().y());
#ifdef MYDEBUG
	cout << "the object's x is " << e->pos().x() << " and " << e->pos().y() << endl;
#endif

}

void View::setCanPress(bool t){
	CanPress = t;
}

void View::contentsMousePressEvent(QMouseEvent *e){
	if (CanPress){
		QCanvasSprite *car1 = new QCanvasSprite(&ani_x,canvas());
		car1->show();
		car1->move(e->pos().x(),e->pos().y());
		car1->setVelocity(-1,0);
	}
#ifdef MYDEBUG
	cout << "the position is " << e->pos().x() << " and " << e->pos().y() << endl;
#endif
}

