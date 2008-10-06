#ifndef __CAR_H__
#define __CAR_H__
#include <qcanvas.h>
#include <qobject.h>

#include "BouncyLogo.h"
#include "InfoElt.h"
#include "global.h"
#include "qcolor.h"

enum CAR_BULB{
	BULB_NONE = 0,
	BULB_GREEN,
	BULB_BLUE ,
	BULB_ORANGE,
	BULB_RED,
	BULB_YELLOW
};


enum DRIVER_BEHAVIOR{
	BCTRL_MODE = 0,         //Behavior Control Mode
	CONSERVATIVE,  //保守
	NORMAL,            //一般  
	AGGRESSIVE        //積極
};
enum CAR_TYPE{
	CT_WIFI = 0,
	CT_DSRC,
	CT_NORMAL 
};


enum CAR_ACC_LEVEL{
	ACC_FULL = 0,      
	ACC ,      
	ACC_REL,      //ACC Releace
	DEC,
	BRAKE_NORMAL,
	BRAKE_EMERG,
	ACTRL_MODE    //ACC Control Mode
};

class NetCoordinator;
class Packet;
class InfoPacket;
class InfoElement;
class QListView;
class Car : public QObject,public BouncyLogo
{
Q_OBJECT
		
	private:
		DRIVER_BEHAVIOR _driver_behavior; //behavior
		CAR_ACC_LEVEL  _acc_level; //accelerator

	public:
		Car(QCanvas*);
		int rtti() const;

		void initPos();
		void initVel();
		void initDriverBeh(); //behavior


// Value Settings & Declare
	// positon (x,y) in real world

	protected:
		virtual void draw( QPainter & p );

	protected:
		double _real_x; //meter
		double _real_y; //meter
		int _node_id;
		int _lan; // 1: 100  2:148  3:196
		bool _relocation; //use for initial location setting
		bool _sel;
		bool _change2R;
		bool _change2L;
		CAR_BULB _front_bulb;
		CAR_BULB _back_bulb;

	public:
		virtual CAR_TYPE CarType(){	return CT_NORMAL;	};

		double realX();
		double realY();
		int lan(){ return _lan; };
		bool sel(){	return _sel;	};
		bool reLocation(){	return _relocation;	};
		bool change2RightLan(){	return _change2R;	};
		bool change2LeftLan(){	return _change2L;	};
		void setRealX(double);
		void setRealY(double);
		void setSel(bool );
		void setLan(int );
		void setReLocation(bool );
		void setChange2RightLan(bool );
		void setChange2LeftLan(bool );
		int nodeID(){	return _node_id;	};
		bool AutoChangeLane(){	return _auto_change_lane;	};
		void setAutoChangeLane(bool b){	_auto_change_lane = b;	};

		CAR_BULB frontBulb(){	return _front_bulb;	};
		CAR_BULB backBulb(){	return _back_bulb;	};
		void setFrontBulb(CAR_BULB b){	_front_bulb = b;	};
		void setBackBulb(CAR_BULB b){	_back_bulb = b;	};

		bool GetCollision();
		void SetCollision();
		bool isCollision();//do not clear

		// in Canvas render // don't need?
		//int _render_x;
		//int _render_y;
		
	private:
		double _real_velocity; // KM/Hour -> for X only now -> in the future maybe Direction Angle
		unsigned int _max_real_velocity; // restrict for Car
		unsigned int _min_real_velocity; // restrict for Car
		
		
		int _start_position; // restrict for Car
		int _end_position; // restrict for Car
	public:
		virtual void calNextVelocity(double);
		double realVelocity();
		void setRealVelocity(double);
		unsigned int maxRealVelocity();
		void setMaxRealVelocity(unsigned int); 
		unsigned int minRealVelocity();
		void setMinRealVelocity(unsigned int); 
		
		unsigned int DriverBehavior();
		void setDriverBehavior(unsigned int); 
		unsigned int AccLevel();
		void setAccLevel(unsigned int); 

		bool _auto_change_lane;
		
		Car* _around_car_array[3][3]; 

		bool _collision;

	/* extension for GPS module */
	public:
		void setGPSUpdateTime(double d){	_gps_next_update_time = d;	};
		double GPSUpdateTime(){	return _gps_next_update_time;	};
		double LastTimeChangeLane(){	return _last_time_change_lane;	};

	private:
		double _gps_next_update_time;
		double _last_time_change_lane;

};

inline void BulbTestSet(CAR_BULB &bulb, CAR_BULB b)
{
	if(b > bulb)
		bulb = b;
}
inline QString Bulb2Str(CAR_BULB b)
{
	switch( b ){
		case BULB_NONE: 
			return "none";
		case BULB_GREEN: 
			return "green";
		case BULB_BLUE: 
			return "blue";
		case BULB_ORANGE: 
			return "orange";
		case BULB_RED:
			return "red";
		case BULB_YELLOW:
			return "yellow";
	}
	return "Not Found";
}
inline QColor Bulb2Color(CAR_BULB b)
{
	 QColor color = Qt::red;
	 switch( b ){
		case BULB_NONE:		break;
		case BULB_BLUE:
			color = Qt::blue; break;
		case BULB_YELLOW:
			color = Qt::yellow; break;
		case BULB_ORANGE:
			color = QColor(255,180,0); break;
		case BULB_RED: 
			color = Qt::red; break;
		case BULB_GREEN: 
			color = Qt::green; break;
		default: break;
	}

	return color;
}

inline QString Acc2Str(CAR_ACC_LEVEL acc)
{
	switch( acc ){
		case ACC_FULL: 
			return "Accelerate Full";
		case ACC: 
			return "Accelerate";
		case ACC_REL: 
			return "Accelerate Release";
		case DEC: 
			return "Decelerate";
		case BRAKE_NORMAL: 
			return "Brake";
		case BRAKE_EMERG:
			return "Brake Emergency!!";
		case ACTRL_MODE:
			return "Control Mode";
	}
	return "Not Found";

}

#endif
