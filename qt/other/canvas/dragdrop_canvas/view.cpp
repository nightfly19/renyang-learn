
#include <qfont.h>
#include <qimage.h>
#include <qdragobject.h>

#include "view.h"

QCanvasPixmapArray View::ani;

View::View(QCanvas &canvas,QWidget *parent):QCanvasView(&canvas,parent)
{
	canvas.setBackgroundPixmap(QPixmap("logo.jpg"));
	canvas.resize(370,300);
	setFixedSize(sizeHint());
	ani.setImage(0,new QCanvasPixmap("cater1.png"));
	setAcceptDrops(true); // 可接收Drop
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

}

