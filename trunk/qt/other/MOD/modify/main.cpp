

/*
 *  MOD -- Messenger On-the-Drive
 *
 *  Copyright (C) 2006 Kai-Fong Chou - <mod.mapleelpam at gmail.com>
 *
 *  http://mod.0rz.net
 *  http://mod.sf.net
 *
 */

#include <qapplication.h>

#include "DrtaMW2.h"

int main(int argc , char** argv)
{
	bool enableGui = true;
	int ret = 0;

	QApplication app( argc, argv);

	if (enableGui)
	{
		DrtaMW2 *drta = new DrtaMW2();
		app.setMainWidget( drta);

		drta -> initDrta(0);
		drta -> show();
		return app.exec();
	}


	return ret;
}
