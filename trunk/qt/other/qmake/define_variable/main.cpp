#include <qapplication.h>
#include <qstring.h>

int main(int argc,char **argv)
{
	QApplication app(argc,argv,false);
#ifdef MYDEFINE
	qWarning("define MYDEFINE");
#else
	qWarning("undefine MYDEFINE");
#endif
	return app.exec();
}
