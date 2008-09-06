
#include <qapplication.h>

#include "view.h"
#include "myqcanvas.h"

int main(int argc,char** argv)
{
	QApplication app(argc,argv);

	MyQCanvas canvas(0,0);

	View c(canvas);
	canvas.setAddress(c.getPolygon(),c.getRectangle(),c.getText());

	canvas.setAdvancePeriod(30);
	canvas.setDoubleBuffering(true);

	app.setMainWidget(&c);
	c.show();

	return app.exec();
}
