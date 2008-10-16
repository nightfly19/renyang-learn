
#ifndef MYQCANVAS_H
#define MYQCANVAS_H

#include "mydebug.h"

#include <qcanvas.h>

class myQCanvas:public QCanvas{
public:
	myQCanvas(int,int);

	void advance();
};

#endif // MYQCANVAS_H

