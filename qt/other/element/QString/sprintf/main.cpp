#include <qapplication.h>
#include <qstring.h>

int main(int argc,char **argv)
{
	QApplication app(argc,argv,false);

	QString str;

	int value =5;

	str.sprintf("the value is %d",value);

	qWarning(str);

	return 0;
}
