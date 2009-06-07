#include <qapplication.h>
#include <qptrdict.h>
#include <qstring.h>

int main(int argc,char **argv)
{
	QApplication app(argc,argv);

	QPtrDict<char> extra;

	QString *str1 = new QString("Hello");

	QString *str2 = new QString("Hello");

	extra.insert(str1,"Surname");

	// 透過str1找出相對應的QString
	qWarning("%s",extra.find(str1));

	// 透過相同的key找出QString, 必需要位址完全相同才可以
	qWarning("%s",extra.find(str2));

	return app.exec();
}
