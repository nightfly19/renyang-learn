#ifndef __COORDINATOR_H__
#define __COORDINATOR_H__

#include <qobject.h>
#include <qcanvas.h>
#include <qtimer.h>
#include "network/net_coor.h"
#include "global.h"

class Car;
class Road;

class Coordinator: public QObject
{
Q_OBJECT
	public:
		Coordinator(QCanvas& ,QObject* parent = 0 , const char* name = 0);

		void add2Coordinator(QCanvasItem* );

		void setRoad(Road* r) { _road = r;	};
		unsigned int CarCount(){	return _car_list.count();	};


		Car* CenterCar(){	return _center_car;	};
		void setCenterCar(Car* c){	_center_car = c;	};

		double CountCarVelocity();

		int CountCollision();
	signals:
		void sigUpdateEnable(bool);
		void simulationDone();


	public slots:
		void clearCarsFlags(); // clear car's flags , sel

		void slotPlay();
		void slotPause();
		void slotStop();
		void car_active(); // call for car_timer

	private slots:
		void gps_active(); // call for gps_timer 
		void ticks_active(); // call for ticks_timer 

	private: 
		QCanvas &canvas;
		QCanvasItemList _car_list;
		Car* _center_car;
		Road* _road;
		QTimer _car_timer;
		QTimer _gps_timer;
		QTimer _ticks_timer;
		Car* _sort_car_array[CAR_MX];

	//當有選擇的時候 不能移動
	public:
		void setSelectItem(QCanvasItem *s) { _select_item = s;	};


	private:
		QCanvasItem* _select_item; 


	//設定Center Car該有的Offset 讓Scroll Position去做設定
	public:
		bool fixedCenter(){	return fixed_center;	};
		void setFixedCenter( bool b){	fixed_center = b;	};
		void slotSetScollPosition(unsigned int pos){	center_pos = pos;	};
	private:
		bool fixed_center;
		unsigned int center_pos;
		bool _lock_try_active_gps_broadcast;

	//private function
	private:
		void Try2MoveOtherCar(Car*);
		void RealSortCarByX();
		void ShellSortCarByX();
		void CountCarsVelocity(double);
		void MoveCarsPosition(double);
		void UpdateAroundCarArray();
		void UpdateFrontCarBulb();
		void UpdateBackCarBulb();
		void UpdateWarnningSystem();
		void UpdateCarBulb();
	public:
		void Try2MoveCenterCar();
};

#endif
