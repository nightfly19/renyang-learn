
#define MYDEBUG

#ifdef MYDEBUG

#include <iostream>
using namespace std;

#endif // MYDEBUG

#include <qfont.h>
#include <qimage.h>

#include "view.h"

View::View(QCanvas &canvas):QCanvasView(&canvas)
{
	canvas.setBackgroundPixmap(QPixmap("logo.jpg"));
	canvas.resize(370,300);
	setFixedSize(sizeHint());
	started=false;
}

void View::contentsMousePressEvent(QMouseEvent *e)
{
	if (e->button()==LeftButton){
		if (!started){
			end = new QPoint(e->x(),e->y());
			started=true;
			#ifdef MYDEBUG	
			cout << end->x() << " and " << end->y() << endl;
			cout << "you are the first time" << endl;
			#endif
		}
		else
		{
			start = end;
			end = new QPoint(e->x(),e->y());
			#ifdef MYDEBUG
			cout << "start is (" << start->x() << "," << start->y() << ") and end is (" << end->x() << "," << end->y() << ")" << endl;
			#endif
			QCanvasLine *i = new QCanvasLine(canvas());
			i->setPoints(start->x(),start->y(),end->x(),end->y());
			i->setPen(blue);
			i->setZ(2);
			i->show();
		}
	}
	else if (e->button()==RightButton)
	{
		started=false;
		QCanvasItemList list=canvas()->allItems();
		for (QCanvasItemList::Iterator it=list.begin();it!=list.end();it++)
		{
			if (*it){
				delete *it;
			}
		}
	}
}
