#include <qapplication.h>
#include <qstring.h>

int main(int argc,char **argv)
{
	QApplication app(argc,argv,false);

	QString str("0123456789");

	// 取出由中間開始, 設定長的字串
	qWarning(str.mid(5,3));

	return 0;
}
