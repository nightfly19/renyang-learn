
#ifndef MYSCENE_H
#define MYSCENE_H

#include <QGraphicsScene>

class QTimeLine;

class MyScene:public QGraphicsScene {
	public:
		MyScene(QObject *parent=0);
		MyScene(qreal x,qreal y,qreal width,qreal height,QObject *parent=0);
		~MyScene();
	private:
		QTimeLine *timeLine;
};

#endif // MYSCENE_H

