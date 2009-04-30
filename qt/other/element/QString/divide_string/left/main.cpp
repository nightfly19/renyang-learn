#include <qapplication.h>
#include <qstring.h>

int main(int argc,char **argv)
{
	QApplication app(argc,argv,false);

	QString str("1234567890");

	// 取出包含最左邊, 長度一共為3的字串
	qWarning(str.left(3));

	return 0;
}
