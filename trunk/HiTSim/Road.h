

#ifndef __ROAD_H__
#define __ROAD_H__

#include <qcanvas.h>
#include <qpainter.h>

#include "global.h"

// Road類別是用來建立道路

class Car;
class Road : public QCanvasRectangle 
{
//Q_OBJECT

	public:
		Road ( QCanvas * canvas );
		Road ( const QRect & r, QCanvas * canvas );
		Road ( int x, int y, int width, int height, QCanvas * canvas ); // 設定道路左上角的起始位置,與道路的長與寬

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

		int _number_roadway; // 記錄道路有幾線道,預設是3線道,改成其它數值只有圖會改變,實際車子還是行走在3線道上
		double _road_width; // 記錄每一個車道的寬度,預設是50
		double _stripe_width; // 記錄道路中間白線的長度
		double _stripe_height; // 記錄道路中間白線的寬度
		Car* _center_car; // 記錄center car
		bool _if_center;
		int _x_pos;
};

#endif

