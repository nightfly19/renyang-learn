#include "MainScene.h"

MainScene::MainScene(QObject *parent):QGraphicsScene(parent)
{
	// do nothing
}

void MainScene::timerEvent(QTimerEvent *event)
{
	// 每隔一段時間畫出所有車子的新位置
}
