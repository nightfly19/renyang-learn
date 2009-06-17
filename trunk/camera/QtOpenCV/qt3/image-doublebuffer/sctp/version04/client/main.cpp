#include <qapplication.h>

#include "MyCameraWindow.h"

int main(int argc,char **argv)
{
	if (argc<2)
	{
		qWarning("%s [server ip]",argv[0]);
		exit(1);
	}
	QApplication app(argc,argv);

	MyCameraWindow *mainWin = new MyCameraWindow(0);

	app.setMainWidget(mainWin);

	mainWin->show();

	// 連到server端
	mainWin->Connect2Host(QString(argv[1]),MY_PORT_NUM);

	// 開始要求傳送圖片
	mainWin->start();

	return app.exec();
}
