
#ifndef MYVIEW_H
#define MYVIEW_H

#include <qcanvas.h>

class MyView:public QCanvasView {
	public:
		MyView(QWidget *parent=0,const char *name=0);
		MyView(QCanvas *canvas,QWidget *parent=0,const char *name=0);
};

#endif
