#include <qapplication.h>
#include "message.h"

int main(int argc,char **argv)
{
	QApplication app(argc,argv,false);

	message *myObject = new message();

	return app.exec();
}
