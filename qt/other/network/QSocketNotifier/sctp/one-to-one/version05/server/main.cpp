#include <qapplication.h>
#include "SCTPServerSocket.h"

int main(int argc,char **argv)
{
	QApplication app(argc,argv,false);

	SCTPServerSocket *server = new SCTPServerSocket(2000,1);

	return app.exec();
}
