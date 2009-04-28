#include <qapplication.h>
#include <qdom.h>
#include <qfile.h>

int main(int argc,char **argv)
{
	// 不顯示GUI
	QApplication app(argc,argv,false);

	// 在程式中建立一個叫"IhuConfigXML"的xml檔
	QDomDocument doc("IhuConfigXML");
	// 建立一個在doc中的成員
	QDomElement root = doc.createElement("renyang");
	// 把root設定為doc目前的最後一個child
	doc.appendChild(root);

	// 建立一個在doc中的成員
	QDomElement general = doc.createElement("general");

	// 設定屬性
	general.setAttribute("myName","fangrenyang");
	general.setAttribute("height","168cm");
	general.setAttribute("weight","70kg");

	QDomElement family = doc.createElement("family");

	family.setAttribute("grandemother",true);
	family.setAttribute("father",true);
	family.setAttribute("mother",true);
	family.setAttribute("old_brother",true);
	family.setAttribute("little_brother",true);

	QDomElement education = doc.createElement("education");

	education.setAttribute("primary_school",true);
	education.setAttribute("secondary_school",true);
	education.setAttribute("high_school",true);
	education.setAttribute("university",true);
	education.setAttribute("graduate_school",true);

	// 設定general, family, education均為root的child
	// 架構圖為doc--root--general
	//                  --family
	//                  --education
	root.appendChild(general);
	root.appendChild(family);
	root.appendChild(education);

	// 開啟檔案
	QFile confFile("confFile.xml");
	// 限定開啟檔案的權限
	if (confFile.open(IO_WriteOnly))
	{
		// 建立一個QTextStream, 提供了基本的讀寫的功能
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
