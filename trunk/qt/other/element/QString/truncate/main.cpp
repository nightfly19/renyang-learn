#include <qapplication.h>
#include <qstring.h>

int main(int argc,char **argv)
{
	QApplication app(argc,argv,false);

	QString str("Hello World!!");
	QString str2("123");

	// 把刪到目前的設定長度
	str.truncate(5);

	// 實際有刪除字串
	qWarning(str);
	// 實際上沒有刪除字串
	qWarning(str2);

	return 0;
}
