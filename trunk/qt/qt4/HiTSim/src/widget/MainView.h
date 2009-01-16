#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>

#include "MainScene.h"

class MainView:public QGraphicsView
{
Q_OBJECT
	public:
		MainView(QWidget *parent=0);
		MainView(QGraphicsScene *scene,QWidget *parent=0);
	public slots:
		void addCar(QGraphicsItem *);
		void addCar();
	private:
		MainScene *scene;
		void init();
	protected:
		void dragEnterEvent(QDragEnterEvent *evnet);
};

#endif // MAINVIEW_H
