
#include <QApplication>

#include "myview.h"
#include "myscene.h"

int main(int argc,char **argv) {

	QApplication app(argc,argv);

	MyScene w(0,0,600,600);

	MyView v(&w,0);

	v.show();

	return app.exec();
}

