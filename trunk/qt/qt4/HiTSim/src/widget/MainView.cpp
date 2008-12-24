#include "MainView.h"
#include "MainScene.h"
#include "mouse.h"

MainView::MainView(QGraphicsScene *scene,QWidget *parent):QGraphicsView(scene,parent)
{
	// do nothing
}

MainView::MainView(QWidget *parent):QGraphicsView(parent)
{
	scene = new MainScene(this);
	scene->setSceneRect(-300,-300,600,600);
	scene->setItemIndexMethod(QGraphicsScene::NoIndex);
	setScene(scene);
	setRenderHint(QPainter::Antialiasing);
	setBackgroundBrush(QPixmap("images/cheese.jpg"));
	setCacheMode(QGraphicsView::CacheBackground);
	//setDragMode(QGraphicsView::ScrollHandDrag);
	resize(400,300);
}

void MainView::addItem(QGraphicsItem *item)
{
	scene->addItem(item);
}

void MainView::addItem()
{
	Mouse *mouse = new Mouse;
	scene->addItem(mouse);
}
