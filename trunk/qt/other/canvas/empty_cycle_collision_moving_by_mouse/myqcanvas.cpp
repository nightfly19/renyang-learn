
#include <qregion.h>

#include "myqcanvas.h"

MyQCanvas::MyQCanvas(int w,int h):QCanvas(w,h)
{
	// do nothing
}

void MyQCanvas::advance()
{
	QRegion region = QRegion(my_p->rect());
	QRegion region1= region.eor(QRegion(my_p1->rect()));
	
	if (region1.contains(my_r->rect()))
		my_t->setText("Collision");
	else
		my_t->setText("There is no Collision!");
	QCanvas::advance();
}

void MyQCanvas::setAddress(QCanvasRectangle* p,QCanvasRectangle* p1,QCanvasRectangle* r,QCanvasText* t)
{
	my_p = p;
	my_p1= p1;
	my_r = r;
	my_t = t;
}
