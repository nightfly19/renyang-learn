#include <qapplication.h>
#include <qstring.h>

int main(int argc,char **argv)
{
	QApplication app(argc,argv,false);

	char str[] = "good";
	char *str2 = str;

	QString qstr(str2);

	// 直接可以把char *指到的字串轉到QString
	qWarning(qstr);

	return 0;
}
