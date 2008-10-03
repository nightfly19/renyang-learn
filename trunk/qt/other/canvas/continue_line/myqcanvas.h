
#ifndef MYQCANVAS_H
#define MYQCANVAS_H

#include <qcanvas.h>

class MyQCanvas:public QCanvas
{
Q_OBJECT
	public:
		MyQCanvas(int w=0,int h=0);

	protected:
		void advance();
};

#endif // MYQCANVAS_H

