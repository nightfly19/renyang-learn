#include <QApplication>
#include <QGraphicsEllipseItem>
#include <QTimeLine>
#include <QtGui>

int main(int argc,char **argv)
{
	QApplication app(argc,argv);

	QGraphicsItem *ball = new QGraphicsEllipseItem(0,0,20,20); // 建立一個球

	QTimeLine *timer = new QTimeLine(5000); // 設定持續時間5000ms
	timer->setFrameRange(0,100); // 設定frame的刻度,由0到100

	QGraphicsItemAnimation *animation = new QGraphicsItemAnimation;
	animation->setItem(ball); // 把ball加入動畫配製中
	animation->setTimeLine(timer); // 設定TimeLine物件去控制動畫的速度

	for (int i=0;i<200;++i)
		animation->setPosAt(i/200.0,QPointF(i, i)); // 設定在某一個step球要到某一個位置,step的數值必需在0(表示一開始)到1(表示完成)之間

	QGraphicsScene *scene = new QGraphicsScene();
	scene->setSceneRect(0, 0, 250, 250);

	scene->addItem(ball);

	QGraphicsView *view = new QGraphicsView(scene);
	view->show();
	timer->start();

	return app.exec();
}
