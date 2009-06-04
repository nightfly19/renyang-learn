#include <qapplication.h>
#include <qdatetime.h>

int main(int argc,char **argv)
{
	QApplication app(argc,argv,false);

	// 取得目前的時間
	QTime currentTime = QTime::currentTime();
	QTime nextTime = currentTime.addMSecs(2365);

	// 印出目前的時間
	qWarning(QString("The current time is %1").arg(currentTime.toString()));

	// 印出2365毫秒之後的時間
	qWarning(QString("The time after 2365ms is %1").arg(nextTime.toString()));

	// 印出這兩個時間的差, 由currentTime與nextTime比較
	qWarning("%d",currentTime.msecsTo(nextTime));

	// 反過來, 由nextTime與currentTime比較
	qWarning("%d",nextTime.msecsTo(currentTime));

	return app.exec();
}
