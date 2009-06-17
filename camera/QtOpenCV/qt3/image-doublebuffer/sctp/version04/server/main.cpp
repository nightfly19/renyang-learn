
#include <qapplication.h>

#include "SctpSocketServer.h"

int main(int argc,char **argv)
{
	QApplication app(argc,argv,false);

	SctpSocketServer server;

	return app.exec();
}
