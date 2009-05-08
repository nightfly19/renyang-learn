#include <qapplication.h>
#include "classA.h"
#include "classB.h"

int main(int argc,char **argv)
{
	QApplication app(argc,argv,false);
	
	classA *a = new classA();
	classB *b = new classB();
	QObject::connect(a,SIGNAL(Signalvalue(int *)),b,SLOT(Slotvalue(int *)));
	a->sendsignal();

	return app.exec();
}
