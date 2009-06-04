#include <qapplication.h>
#include <qdatetime.h>

int main(int argc,char **argv)
{
	QApplication app(argc,argv,false);

	QTime time;

	// 開始計時
	time.start();

	// 休息1秒
	sleep(1);

	// 回傳由開始到目前過了多少毫秒
	qWarning("%d",time.elapsed());

	// 再休息一秒
	sleep(2);

	// 回傳由開始到目前過了多少毫秒, 並把計數器值設定為0
	qWarning("%d",time.restart());

	// 嘗試看看, 計數器值是否有設定為0
	qWarning("%d",time.elapsed());

	return app.exec();
}
