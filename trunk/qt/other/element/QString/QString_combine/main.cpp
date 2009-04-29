#include <qapplication.h>
#include <qstring.h>

int main(int argc,char **argv)
{
	// 不使用GUI介面
	QApplication app(argc,argv,false);

	char str1[]="Hello";
	char str2[]="World!";
	QString fileName;
	// 把所有的字串, 串在一起
	fileName = QString("%1 %2").arg(str1).arg(str2);
	qDebug(fileName);

	return 0;
}
