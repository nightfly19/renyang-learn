#include <qapplication.h>
#include <qlabel.h>
#include <qimage.h>
#include <qpixmap.h>

int main(int argc,char *argv[]) {
	QApplication app(argc,argv);

	QColor color(255,0,0);	// 設定顏色
	QLabel label(0);
	label.resize(QSize(100,100));	// 設定label的大小
	QPixmap p(10,10);	// 設定顏色區塊的大小
	p.fill(color);		// 用顏色把pixmap填滿
	label.setPixmap(p);	// 用此pixmap設定此label
	label.show();
	app.setMainWidget(&label);

	return app.exec();
}
