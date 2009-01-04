
#include <QApplication>

#include "toolbox.h"

int main(int argc,char **argv) {

	QApplication app(argc,argv);

	MyToolBox w;
	w.show();

	return app.exec();
}

