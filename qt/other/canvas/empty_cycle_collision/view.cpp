
#include <qfont.h>
#include <qimage.h>

#include "view.h"

View::View(QCanvas &canvas):QCanvasView(&canvas)
{
	canvas.setBackgroundPixmap(QPixmap("logo.jpg"));
	canvas.resize(370,300);
	setFixedSize(sizeHint());

	p = new QCanvasRectangle(100,100,100,100,&canvas);
	p->setBrush(NoBrush);
	p->setPen(Qt::blue);
	p->setZ(10);
	p->show();

	r = new QCanvasRectangle(100,100,5,10,&canvas);
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
	r->move(e->pos().x(),e->pos().y());
}

QCanvasRectangle* View::getPolygon() const
{
	return p;
}

QCanvasRectangle* View::getRectangle() const
{
	return r;
}

QCanvasText* View::getText() const
{
	return t;
}
