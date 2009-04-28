#include <qapplication.h>
#include <qdom.h>
#include <qfile.h>

int main(int argc,char **argv)
{
	// 不顯示GUI
	QApplication app(argc,argv,false);

	QDomDocument doc("IhuConfigXML");
	QDomElement root = doc.createElement("renyang");
	doc.appendChild(root);

	QDomElement general = doc.createElement("general");

	general.setAttribute("myName","fangrenyang");
	general.setAttribute("height","168cm");
	general.setAttribute("weight","70kg");

	QDomElement family = doc.createElement("family");

	family.setAttribute("grandemother",true);
	family.setAttribute("father",true);
	family.setAttribute("mother",true);
	family.setAttribute("old brother",true);
	family.setAttribute("little brother",true);

	QDomElement education = doc.createElement("education");

	education.setAttribute("primary school",true);
	education.setAttribute("secondary school",true);
	education.setAttribute("high school",true);
	education.setAttribute("university",true);
	education.setAttribute("graduate school",true);

	root.appendChild(general);
	root.appendChild(family);
	root.appendChild(education);

	QFile confFile("confFile.xml");
	if (confFile.open(IO_WriteOnly))
	{
		QTextStream ts(&confFile);
		ts << doc.toString();
		qDebug("create xml success!!");
		confFile.close();
	}
	else
	{
		qDebug(QString("can't write config file %1 (%2)").arg("confFile.xml").arg(confFile.errorString()));
	}

	return 0;
}
