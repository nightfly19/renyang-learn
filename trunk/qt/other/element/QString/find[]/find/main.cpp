#include <qapplication.h>
#include <qstring.h>

int main(int argc,char **argv)
{
	QApplication app(argc,argv,false);

	QString str("Hello! World!!");

	int tmpInd = str.find(':');
	
	if (tmpInd > 0)
	{
		qWarning("We can find that");
	}
	else
		qWarning("We can't find that");

	// 由最左邊往右邊開始找
	// index由左而右是0,1,2,3...
	// 當index預設由-1開始找, 也就是由最左邊往最右邊找
	tmpInd = str.find('!');
	if (tmpInd > 0)
	{
		qWarning(QString("the character appear at the %1").arg(tmpInd));
	}
	else
		qWarning("We can't find the character");
	
	return 0;
}
