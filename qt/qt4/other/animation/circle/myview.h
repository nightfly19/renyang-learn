
#ifndef MYVIEW_H
#define MYVIEW_H

#include <QGraphicsView>

class MyScene;

class MyView:public QGraphicsView {
	public:
		MyView(QWidget *parent=0);
		MyView(MyScene *scene,QWidget *parent=0);
};

#endif // MYVIEW_H

