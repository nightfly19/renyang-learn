#include <qapplication.h>
#include <qstring.h>

int main(int argc,char **argv)
{
	QApplication app(argc,argv,false);

	QString str("Hello World!!");

	qWarning(str);

	// 透過latin1()轉換過去後, 並不包含換行符號
	// 但是最後還是會補一個'\0'的字元
	// 所以判斷長度還是不會有問題
	printf("%s",str.latin1());

	printf("%d\n",strlen(str.latin1()));

	return 0;
}
