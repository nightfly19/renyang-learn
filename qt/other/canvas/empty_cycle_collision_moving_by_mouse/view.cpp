
#include <qfont.h>
#include <qimage.h>

#include "view.h"

View::View(QCanvas &canvas):QCanvasView(&canvas)
{
	canvas.setBackgroundPixmap(QPixmap("logo.jpg"));
	canvas.resize(370,300);
	setFixedSize(sizeHint());

	empty_Rectangle(100,100,100,1);

	r = new QCanvasRectangle(100,100,25,20,&canvas);
	r->setVelocity(0.5,0);
	r->setBrush(Qt::red);
	r->setZ(10);
	r->show();

	t = new QCanvasText("What will you",&canvas);
	t->setFont(QFont("Helvetica",12,QFont::Bold));
	t->setColor(Qt::blue);
	t->setZ(20);
	t->setTextFlags(AlignBottom);
	t->move(5,20);
	t->show();
}

void View::contentsMousePressEvent(QMouseEvent *e)
{
	if (e->button()==LeftButton)
	{
		r->move(e->pos().x(),e->pos().y());
		moving = 0;
	}
	else if (e->button()==RightButton)
	{
		QPoint p=e->pos();
		QCanvasItemList l=canvas()->collisions(p);
		for (QCanvasItemList::Iterator it=l.begin();it!=l.end();++it){
			moving = *it;
			moving_start = p;
			moving->setAnimated(false);
			return;
		}
		moving=0;
	}
}

void View::contentsMouseMoveEvent(QMouseEvent *e)
{
	if (moving){
		QPoint p = e->pos();
		moving->moveBy(p.x()-moving_start.x(),p.y()-moving_start.y());
		moving_start = p;
		canvas()->update();
	}
}

void View::contentsMouseReleaseEvent(QMouseEvent *e)
{
	if (moving){
		moving->setAnimated(true);
	}
}

QCanvasRectangle* View::getPolygon() const
{
	return p;
}

QCanvasRectangle* View::getPolygon1() const
{
	return p1;
}

QCanvasRectangle* View::getRectangle() const
{
	return r;
}

QCanvasText* View::getText() const
{
	return t;
}

QCanvasRectangle *View::Rectangle(int x,int y,int size) const
{
	QCanvasRectangle *temp = new QCanvasRectangle(x-size/2,y-size/2,size,size,canvas());
	return temp;
}

void View::empty_Rectangle(int x,int y,int size,int thickness)
{
	p = Rectangle(x,y,size);
	p->setBrush(NoBrush);
	p->setPen(Qt::blue);
	p->setZ(10);
	p->show();

	p1 = Rectangle(x,y,size-2*thickness);
	p1->setBrush(NoBrush);
	p1->setPen(Qt::blue);
	p1->setZ(10);
	p1->show();
}
