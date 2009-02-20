#include <qapplication.h>

#include "MyWidget.h"

int main(int argc,char **argv) {
	QApplication app(argc,argv);

	MyWidget w;

	w.show();

	app.setMainWidget(&w);

	return app.exec();
}
