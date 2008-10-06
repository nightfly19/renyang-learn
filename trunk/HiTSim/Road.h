

#ifndef __ROAD_H__
#define __ROAD_H__

#include <qcanvas.h>
#include <qpainter.h>

#include "global.h"


class Car;
class Road : public QCanvasRectangle 
{
//Q_OBJECT

	public:
		Road ( QCanvas * canvas );
		Road ( const QRect & r, QCanvas * canvas );
		Road ( int x, int y, int width, int height, QCanvas * canvas );

		int rtti() const;

		void setIfCenter(bool c){	_if_center = c;	};
		bool IfCenter(){	return _if_center;	};

		void setXPos(int x){	_x_pos = x;	};
		int xPos(){	return _x_pos;	};

	protected:
		virtual void drawShape ( QPainter & p );


	// 道路的參數

	public:
		int numberRoadway()	{	return _number_roadway;	};
		double roadWidth()	{	return _road_width;	};
		double stripeWidth()	{	return _stripe_width;	};
		double stripeHeight()	{	return _stripe_height;	};

		void setNumberRoadway(int v)	{	_number_roadway = v;	};
		void setRoadWidth(double v)	{	_road_width = v;	};
		void setStripeWidth(double v)	{	_stripe_width = v;	};
		void setStripeHeight(double v)	{	_stripe_height = v;	};
//	public slots:
		void slotCenterCarChange(Car*);
		void repaint(){		update();	};

	private:
		void init();

		int _number_roadway;
		double _road_width;
		double _stripe_width;
		double _stripe_height;
		Car* _center_car;
		bool _if_center;
		int _x_pos;
};

#endif

