
#include <QApplication>
#include <QtGui>

int main(int argc,char **argv) {
	QApplication app(argc,argv);

	QGraphicsScene scene;
	// 要把widget嵌入到QGraphicsScene內,必需要透過QGraphicsProxyWidget才可以
	// addWidget回傳的是QGraphicsProxyWidget,由QGraphicsWidget繼承
	QGraphicsWidget *textEdit = scene.addWidget(new QTextEdit);
	QGraphicsWidget *pushButton = scene.addWidget(new QPushButton);

	QGraphicsGridLayout *layout = new QGraphicsGridLayout;
	// QGraphicsGridLayout的addItem函數必需是QGraphicsWidget
	layout->addItem(textEdit, 0, 0);
	layout->addItem(pushButton, 0, 1);

	QGraphicsWidget *form = new QGraphicsWidget;
	form->setLayout(layout);
	scene.addItem(form);

	QGraphicsView view(&scene);

	view.show();


	return app.exec();
}

