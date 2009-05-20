#include <qapplication.h>

#include "MyCameraWindow.h"

int main(int argc,char **argv)
{
	QApplication app(argc,argv);

	MyCameraWindow *mainWin = new MyCameraWindow(0);

	app.setMainWidget(mainWin);

	mainWin->show();

	mainWin->Connect2Host(QString("127.0.0.1"),MY_PORT_NUM);

	// 開始要求傳送圖片
	mainWin->startVideo();

	return app.exec();
}
