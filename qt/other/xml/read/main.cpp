#include <qapplication.h>
#include <qfile.h>
#include <qdom.h>
#include <qstring.h>

int main(int argc,char **argv)
{
	QApplication app(argc,argv,false);

	// 用來記錄是否讀取xml檔案成功
	bool ret = false;

	QDomDocument doc("IhuConfigXML");

	QString fileName("confFile.xml");

	QFile confFile(fileName);

	if (confFile.open(IO_ReadOnly))
	{
		// This function reads the XML document from the IO device dev
		if (doc.setContent(&confFile))
		{
			// 設定文件標頭為一開始的最上層tag
			QDomElement root = doc.documentElement();
			if (root.tagName() == "renyang")
			{
				// 取得第一個child tage
				QDomNode n = root.firstChild();
				while (!n.isNull())
				{
					QDomElement e = n.toElement();
					if (!e.isNull())
					{
						QString tagName = e.tagName();
						if (tagName == "general")
						{
							qDebug(QString("myName: %1").arg(e.attribute("myName",QString(""))));
						}
						else if (tagName == "family")
						{
							// 這裡會找不到grandemother, 因為實際檔案中grandmother多打了一個e
							// 就會回傳後面的預設值
							qDebug(QString("grandmother: %1").arg(e.attribute("grandmother","99")));
						}
						else if (tagName == "education")
						{
							qDebug(QString("primary_school: %1").arg(e.attribute("primary_school","99")));
						}
						else
						{
							// do nothing
						}
					}
					// 下一個tag
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
	return ret;
}
