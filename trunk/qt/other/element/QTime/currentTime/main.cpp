#include <qapplication.h>
#include <qdatetime.h>

int main(int argc,char **argv)
{
	QApplication app(argc,argv,false);

	QTime currentTime;

	currentTime = QTime::currentTime();
	// 顯示目前的時間hh:mm::ss
	sleep(1);

	qWarning(currentTime.toString());

	qWarning("%d",currentTime.msecsTo(QTime::currentTime()));

	return app.exec();
}
