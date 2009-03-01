#include <qapplication.h>
#include "MyCameraWindow.h"

int main(int argc,char **argv)
{
	QApplication app(argc,argv);
	MyCameraWindow *mainWin = new MyCameraWindow(0);

	mainWin->show();

	app.setMainWidget(mainWin);

	return app.exec();
}
