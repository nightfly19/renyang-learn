#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>

int main(int argc,char **argv)
{
	QApplication app(argc,argv);

	QGraphicsScene scene;
	scene.setSceneRect(0,0,600,600); // 設定畫布位置在(0,0)且長寬為600*600
	scene.setItemIndexMethod(QGraphicsScene::NoIndex);
	QGraphicsView view(&scene); // 設定視窗,且其畫布為scene
	view.setRenderHint(QPainter::Antialiasing); // 設定加速QPainter的引擎
	view.setBackgroundBrush(QPixmap("./images/cheese.jpg")); // 設定背景
	view.setCacheMode(QGraphicsView::CacheBackground); // 背景預先被存起來,加速畫面的速度
	view.setDragMode(QGraphicsView::ScrollHandDrag); // 當按滑鼠左鍵時,移動滑鼠會移動捲軸
	view.setWindowTitle(QT_TRANSLATE_NOOP(QGraphicsView, "Colliding Mice")); //設定視窗標題欄的名稱
	view.resize(400, 300); // 設定可見視窗的大小
	view.show(); // 設定可視

	return app.exec();
}
