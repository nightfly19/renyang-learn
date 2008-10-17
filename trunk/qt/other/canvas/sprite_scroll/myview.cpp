#include "caterpillar.h"
#include "myview.h"

View::View(myQCanvas& canvas):QCanvasView(&canvas){
	canvas.resize(2300,2200);
	setFixedSize(sizeHint());

	Caterpillar *cater1 = new Caterpillar(&canvas);
	cater1->move(1250,1150);
	cater1->setVelocity(1,0);
	cater1->setZ(10);
	cater1->show();

	cater2 = new Caterpillar(&canvas);
	cater2->move(1250,1100);
	cater2->setVelocity(1.5,0);
	cater2->setZ(10);
	cater2->show();

	control_center();
	connect(this->canvas(),SIGNAL(move_center_signals()),SLOT(control_center()));
}

void View::control_center(){
	center((int)cater2->x(),(int)cater2->y());
}
