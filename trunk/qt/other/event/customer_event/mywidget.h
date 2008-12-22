#ifndef  MYWIDGET_H
#define  WYWIDGET_H

#include <qwidget.h>
#include "myevent.h"

class MyWidget:public QWidget
{
	public:
		MyWidget(QWidget *parent,const char *name=0);
	
	protected:
		void customEvent(QCustomEvent *);

};

#endif // MYWIDGET_H
