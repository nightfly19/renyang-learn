#include <qapplication.h>
#include "SCTPClientSocket.h"

int main(int argc,char **argv)
{
	if (argc < 2) {
		qWarning("%s serverip",argv[0]);
		exit(1);
	}

	QApplication app(argc,argv,false);

	SCTPClientSocket *client = new SCTPClientSocket();

	QObject::connect(client,SIGNAL(serverclosed()),qApp,SLOT(quit()));

	client->connectToHost(argv[1],2000);

	client->sendMsg(QString("Hello World!!"));

	return app.exec();
}
