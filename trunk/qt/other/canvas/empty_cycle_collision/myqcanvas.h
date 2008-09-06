
#ifndef MYQCANVAS_H
#define MYQCANVAS_H

#include <qcanvas.h>

class MyQCanvas:public QCanvas
{
Q_OBJECT
	public:
		MyQCanvas(int w=0,int h=0);
		void setAddress(QCanvasRectangle*,QCanvasRectangle*,QCanvasText*);

	protected:
		void advance();

	private:
		QCanvasRectangle *my_p;
		QCanvasRectangle *my_r;
		QCanvasText *my_t;
};

#endif // MYQCANVAS_H

