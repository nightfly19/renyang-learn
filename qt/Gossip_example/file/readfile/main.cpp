#include <qstringlist.h>
#include <qfile.h>
#include <iostream>

using namespace std;

int main(){
	QStringList lines;
	QFile file("main.cpp");
	if (file.open(IO_ReadOnly)){
		QTextStream stream(&file);
		QString line;
		int i=1;
		while (!stream.atEnd()){
			line = stream.readLine(); // line of text excluding '\n'
			printf("%3d:%s\n",i++,line.latin1());
			lines += line;
		}
		file.close();
	}
	return 0;
}
