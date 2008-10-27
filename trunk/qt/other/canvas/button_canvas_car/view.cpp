
#include <qfont.h>
#include <qimage.h>
#include <qdragobject.h>
#include <time.h>

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
		srand((unsigned)time(NULL));
	canvas.resize(1370,1300);
	setFixedSize(sizeHint());
	ani.setImage(0,new QCanvasPixmap("cater1.png"));
	ani_x.setImage(0,new QCanvasPixmap("cater2.png"));
	setAcceptDrops(true); // 可接收Drop
	CanPress = false;
	moving = 0;
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
	car1->setVelocity(-rand()%10-1,0);
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
		car1->setVelocity(-rand()%10-1,0);
	}
	else{
		QPoint p = e->pos();
		QCanvasItemList l = canvas()->collisions(p);
		for (QCanvasItemList::Iterator it=l.begin();it!=l.end();++it){
			moving = *it;
			moving_start = p;
			moving->setAnimated(false);
			return;
		}
		moving = 0;
	}
#ifdef MYDEBUG
	cout << "the position is " << e->pos().x() << " and " << e->pos().y() << endl;
#endif
}

void View::contentsMouseMoveEvent(QMouseEvent *e){
	if (!CanPress && moving){
		QPoint p = e->pos();
		moving->moveBy(p.x()-moving_start.x(),p.y()-moving_start.y());
		moving_start = p;
		canvas()->update();
	}
}

void View::contentsMouseReleaseEvent(QMouseEvent *){
	if (!CanPress && moving){
		moving->setAnimated(true);
	}
}

