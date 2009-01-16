#include "MainView.h"
#include "MainScene.h"
#include "Car.h"

#include <QMouseEvent>
#include <QDrag>

MainView::MainView(QGraphicsScene *scene,QWidget *parent):QGraphicsView(scene,parent)
{
	setAcceptDrops(true);	// 讓view可以接受drop
}

MainView::MainView(QWidget *parent):QGraphicsView(parent)
{
	init();
	scene = new MainScene(this);
	scene->setSceneRect(-3000,-3000,6000,6000); // 左上角座標是(-300,-300)
	scene->setItemIndexMethod(QGraphicsScene::NoIndex);
	setScene(scene);
	setRenderHint(QPainter::Antialiasing);
	setBackgroundBrush(QPixmap(":/cheese.jpg"));
	setCacheMode(QGraphicsView::CacheBackground);
	//setDragMode(QGraphicsView::ScrollHandDrag);
	resize(400,300);
}

void MainView::addCar(QGraphicsItem *item)
{
	scene->addItem(item);
}

void MainView::addCar()
{
	Car *car = new Car;
	scene->addItem(car);
}

void MainView::init() {
	// setRenderHint(QPainter::Antialiasing, true); // 讓畫面變的比較平滑
}

void MainView::dragEnterEvent(QDragEnterEvent *event) {
	// do nothing
}
