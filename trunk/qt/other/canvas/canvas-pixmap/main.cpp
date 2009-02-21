#include <qapplication.h>

#include "MyView.h"
#include "MyCanvas.h"

int main(int argc,char **argv) {
	QApplication app(argc,argv);

	MyCanvas canvas;
	MyView view(&canvas);
	app.setMainWidget(&view);
	view.show();

	return app.exec();
}
