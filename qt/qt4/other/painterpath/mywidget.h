
#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>

class MyWidget:public QWidget {
	public:
		MyWidget(QWidget *parent=0);
	protected:
		void paintEvent(QPaintEvent *);

};

#endif // MYWIDGET_H

