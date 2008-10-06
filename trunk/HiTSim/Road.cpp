
#include <qpainter.h>
#include "Car.h"
#include "Road.h"

Road :: Road ( QCanvas * canvas )
	: QCanvasRectangle(canvas)
	, _center_car(NULL)
	, _if_center(true)
	, _x_pos(0)
{
	setX(0);
	setY(0);
	setSize(canvas->width(), canvas->height() );

	init(); 
}

Road :: Road ( const QRect & r, QCanvas * canvas )
	: QCanvasRectangle(r,canvas)
{
	init(); 
}

Road :: Road ( int x, int y, int width, int height, QCanvas * canvas )
	: QCanvasRectangle(x,y,width,height,canvas)
{
	init();
}

void Road :: init()
{
	_number_roadway = 3;
	_road_width = 50;
	_stripe_width = 80;
	_stripe_height = 4;

}

int Road :: rtti() const
{
	return ROAD_RTTI;
}

void Road :: drawShape ( QPainter & p )
{
/*	int cX = int( x() + (width()/2) );
	int cY = int( y() + (height()/2) );*/
	double x_offset = 0;
	p . setBrush( white );

	if(_center_car && _if_center){
		x_offset = int(-_center_car->realX()) % int(_stripe_width*2) + 1.1;
//		debug("_center_car->x() = %lf x_offset = %d",_center_car->realX(),x_offset);
	}else if( !_if_center){
		x_offset = _x_pos*10 % int(_stripe_width*2) + 1.1;
	}else{
		//debug("no center car");
	}
	//畫出Timer Ticks
	 p.setBrush(black);
	 p.setPen(black);
	 //double tmp = (double(g_timer_ticks)*((g_ticks_timer_gap)/1000.0));
	 double tmp = (double(g_timer_ticks))/1000.0;
	 p.drawText(15,15,QString::number(tmp));

	// 整條路灰色 
	p . setBrush( gray );
	int gY = (int)(( y() + (height()/2.0)) - (_road_width*(_number_roadway)/2.0));
	int gH = (int)_road_width*(_number_roadway);
	p . drawRect( 0 , gY, int(canvas()->width()), gH ); 

	p . setBrush( white );
	// 一條一條的
	for(int i = 0; i < _number_roadway; i++){
		int nY = (int) (gY + _road_width*i);
		if(nY == gY || nY == gH+gY){
			p . drawRect( 0 , nY, int(canvas()->width()), int(_stripe_height)); 
		}else{
			for(int ii = 0 ; ; ii ++){
				int nX = (int)((_stripe_width*2)*ii + x_offset);
				p . drawRect( nX, nY, int(_stripe_width), int(_stripe_height));
				if(ii*_stripe_width*2 >canvas()->width())
					break;
			}
		}

	} 
// 畫路標.... 10pixel = 1m .. 所以每10m就畫一個
	if(_center_car && _if_center){
		x_offset = int(-_center_car->realX()) % int(10*10);
	}else{
		x_offset = int(-_x_pos) % int(10*10); 
	}

//
	int nX = (int)x_offset;

/*
	double rX = 0;
	if(_center_car)
		rX = (_center_car->x() - x_offset) + _center_car->realX();
	else
		rX = 1000;
	if(_center_car)
	debug("rx=%lf nX=%lf cx=%lf  crx=%lf\n",rX,nX,_center_car->x(),_center_car->realX());
	*/

	for(int ii = 0 ; ; ii ++){
		nX = (10*10)*ii + int(x_offset);
		p . drawRect( nX, gY , 2 , -10);

		//Real X 即路標
//		rX -= (10);
//		p.drawText(nX-3,gY-10,QString::number(int(rX))); 

		if(ii*10*10 >canvas()->width())
			break;
	}


}

void Road::slotCenterCarChange(Car* c)
{
	_center_car = c;
}
