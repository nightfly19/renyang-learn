
#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <qwidget.h>

class MyWidget:public QWidget {
	Q_OBJECT
	public:
		MyWidget(QWidget *parent=0,const char *name=0);
	protected:
		void paintEvent( QPaintEvent * );
};

#endif // MYWIDGET_H
