
#include <qcanvas.h>
#include "caterpillar.h"
#include "myview.h"

View::View(QCanvas& canvas):QCanvasView(&canvas){
	canvas.resize(300,200);
	setFixedSize(sizeHint());

	Caterpillar *cater1 = new Caterpillar(&canvas);
	cater1->move(250,50);
	cater1->setVelocity(-1,0);
	cater1->setZ(10);
	cater1->show();

	Caterpillar *cater2 = new Caterpillar(&canvas);
	cater2->move(250,100);
	cater2->setVelocity(-1.5,0);
	cater2->setZ(10);
	cater2->show();
}

