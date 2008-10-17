
#ifndef MYQCANVAS_H
#define MYQCANVAS_H

#include "mydebug.h"

#include <qcanvas.h>

class myQCanvas:public QCanvas{
Q_OBJECT
public:
	myQCanvas(int,int);
signals:
	void move_center_signals();
protected:
	void advance();
};

#endif // MYQCANVAS_H

