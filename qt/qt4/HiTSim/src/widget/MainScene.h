#ifndef MAINSCENE_H
#define MAINSCENE_H

#include <QGraphicsScene>

class MainScene:public QGraphicsScene
{
	public:
		MainScene(QObject *parent=0);
	
	protected:
		void timerEvent(QTimerEvent *event);
};

#endif // MAINSCENE_H
