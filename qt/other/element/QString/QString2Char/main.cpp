#include <qapplication.h>
#include <qstring.h>

int main(int argc,char **argv)
{
	QApplication app(argc,argv,false);

	QString str("Hello World!!");

	qWarning(str);

	// 透過latin1()轉換過去後, 並不包含換行符號
	printf("%s\n",str.latin1());

	return 0;
}
