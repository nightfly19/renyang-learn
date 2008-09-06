
#include "myqcanvas.h"

MyQCanvas::MyQCanvas(int w,int h):QCanvas(w,h)
{
	// do nothing
}

void MyQCanvas::advance()
{
	if (my_r->collidesWith(my_p))
	        my_t->setText("Collision");
       	else
               	my_t->setText("There is no Collision!");
	
	QCanvas::advance();
}

void MyQCanvas::setAddress(QCanvasPolygon* p,QCanvasRectangle* r,QCanvasText* t)
{
	my_p = p;
	my_r = r;
	my_t = t;
}
