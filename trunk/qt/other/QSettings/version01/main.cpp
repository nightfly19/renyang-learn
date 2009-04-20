#include <qsettings.h>
#include <qapplication.h>
#include <qstringlist.h>

int main(int argc,char **argv)
{
	QApplication app(argc,argv);

	bool gui=false;
	bool ret=false;
	int width = 88;
	QString bgColor;
	QStringList keys;
	QValueList<QSettings>::iterator it;	// 用來放QSettings的容器

	QSettings settings;
	// settings.insertSearchPath(QSettings::Windows,"MyCompany");
	// No search path needed for Unix; see notes further on.
	// Use default values if the keys don't exist

	// 可以把後面的數值寫到settings的這一個key項目中
	// 1. 設定第一個項目"/MyApplication/geometry/width"
	settings.writeEntry("/MyApplication/geometry/width",width);

	// 讀取相對應的key,且讀出來的是字串型態
	// 2. 因之前沒有設定過,所以目前是設定第二個項目"/MyApplication/background color"
	// 字串的部分一直轉換!!失敗!!,所以在search時,也不會找到這一個key
	bgColor = settings.readEntry("/MyApplication/background color","white",&ret);
	if (!ret)
		qDebug("1. transfer error");

	// 若沒有找到則以後面的default value設定"MyApplication/geometry/width"為相對應的值,並回傳回來
	// 以下的例子,若之前沒有設定"MyApplication/geometry/width"的相對應的值
	// 則"MyApplication/geoetry/width"會被設定為640
	width = settings.readNumEntry("/MyApplication/geometry/width",640,&ret);

	qDebug("%d",settings.readNumEntry("/MyApplication/geometry/width",55,&ret));

	keys = settings.entryList("/MyApplication/geometry");

	qDebug("%d",keys.count());

	return gui?app.exec():0;
}
