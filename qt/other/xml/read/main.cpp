#include <qapplication.h>
#include <qfile.h>
#include <qdom.h>
#include <qstring.h>

int main(int argc,char **argv)
{
	QApplication app(argc,argv,false);

	// 用來記錄是否讀取xml檔案成功
	bool ret = false;

	QDomDocument doc("ConfigXML");

	QString fileName("confFile.xml");

	QFile confFile(fileName);

	if (!confFile.open(IO_ReadOnly))
	{
		qDebug("open file error");
		return 0;
	}
	if (!doc.setContent(&confFile))
	{
		qDebug("setContent error");
		confFile.close();
		return 0;
	}

	/*
	if (confFile.open(IO_ReadOnly))
	{
		// This function reads the XML document from the IO device dev
		if (doc.setContent(&confFile))
		{
			QDomElement root = doc.documentElement();
			if (root.tagName() == "renyang")
			{
				QDomNode n = root.firstChild();
				while (!n.isNull())
				{
					QDomElement e = n.toElement();
					if (!e.isNull())
					{
						QString tagName = e.tagName();
						if (tagName == "general")
						{
							// do something
						}
						else if (tagName == "family")
						{
							// do something
						}
						else if (tagName == "education")
						{
							// do something
						}
						else
						{
							// do nothing
						}
					}
					n = n.nextSibling();
				}
				ret = true;
			}
			else
			{
				qWarning(QString("Error: %1 is not a valid config file").arg("confFile.xml"));
			}
			confFile.close();
		}
		else
		{
			qWarning(QString("Error: error occurred while parsing %1").arg("confFile.xml"));
		}
	}
	else
	{
		qWarning(QString("Error: can't read config file %1 (%2)").arg("confFile.xml").arg(confFile.errorString()));
	}
	*/
	return ret;
}
