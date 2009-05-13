#include <qapplication.h>
#include <qstringlist.h>
#include <iostream.h>
using namespace std;

int main(int argc,char **argv)
{
	QStringList fonts;
	// 加入字串有以下三種用法
	fonts.append("Times");
	fonts += "Courier";
	fonts += "Courier New";
	fonts << "Helvetina [Cronyx]" << "Helevtica [Adobe]";

	for (QStringList::Iterator it = fonts.begin();it != fonts.end();++it)
	{
		cout << *it << ":";
	}
	cout << endl;

	return 0;
}
