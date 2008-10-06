
#include <qpainter.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qimage.h>

#include "global.h"
#include "Car.h"

static QCanvasPixmapArray car_logo_r;
static QCanvasPixmapArray car_logo_g;
static QCanvasPixmapArray car_logo_b;
static QCanvasPixmapArray car_logo_y;

Car::Car( QCanvas* canvas)
	: BouncyLogo( canvas )
	, _driver_behavior(NORMAL)
	, _acc_level(ACC_REL)
	, _real_x(0), _real_y(0) 
	, _real_velocity(gMinVelocity)
	, _max_real_velocity(gMaxVelocity)
	, _min_real_velocity(gMinVelocity)
	, _auto_change_lane(false)
	, _collision(false)
	, _gps_next_update_time(0)
	, _last_time_change_lane(0)
//	, pos_list ()
{
	//_real_velocity = _min_real_velocity = gMinVelocity;
	_relocation = false;
	_sel = false;
	_change2R = false;
	_change2L = false;
        
	_start_position = gStartPosition * 1000 * 10; //pixel
	_end_position = gEndPosition * 1000 * 10;
	
	_node_id = gMaxNodeID ++;
	_front_bulb = BULB_NONE;
	_back_bulb = BULB_NONE;

	//initPos(); //可以的話不應該在這呼叫
	initVel(); //可以的話不應該在這呼叫
	initDriverBeh(); //可以的話不應該在這呼叫
    
	//如果BouncyLogo要開動畫的話記得要改掉這一行
	setAnimated(FALSE);

	//pos_list . setAutoDelete( true );	

	static bool run_once = false;
	if( run_once == false){
		/* load to memory */
		static QCanvasPixmap pix_car_logo_r(QImage::fromMimeSource("car_r.png"));
		static QCanvasPixmap pix_car_logo_g(QImage::fromMimeSource("car_g.png"));
		static QCanvasPixmap pix_car_logo_b(QImage::fromMimeSource("car_b.png"));
		static QCanvasPixmap pix_car_logo_y(QImage::fromMimeSource("car_y.png"));
		/* load to memory */
		car_logo_r . setImage(0, &pix_car_logo_r );
		car_logo_g . setImage(0, &pix_car_logo_g );
		car_logo_b . setImage(0, &pix_car_logo_b );
		car_logo_y . setImage(0, &pix_car_logo_y );
		run_once = true;
	}
}


int Car::rtti() const
{
    return CAR_RTTI;
}

void Car::initPos()
{
	/*
	//BouncyLogo::initPos();
        int i=0;

//set random _real_y

	i = (rand()%(290))+1;
	_real_y = i;

	if( _real_y < 100)
	{
		_real_y = 100;
		_lan = 1;
	}
	else if ( _real_y > 196)
	{
		_real_y =196;
		_lan = 3;
	}
	else	
	{
		_real_y = 148;
		_lan = 2;
	}

	if (_node_id == 1){
		_real_y = 148;
	}
		
//	printf("_real_y = %f \n ", _real_y);

//Set random _real_x

	i = (rand()%(_start_position-_end_position+1))+_end_position;
	_real_x = i;

	if (_node_id == 1)
		_real_y = 148;

	//printf("_real_x = %f \n ", _real_x);
	*/
}

void Car::initVel()
{
	int i=0;
	
//	while( i<_min_real_velocity || i>_max_real_velocity)
//		i = rand();
	
	i = (rand()%(_max_real_velocity-_min_real_velocity+1))+_min_real_velocity;
	//_real_velocity = i;
	_real_velocity = i;
	//printf("_real_velocity = %d \n ", i);
}

void Car::initDriverBeh()
{
        int i=0;
	/*
	static QValueList<QPixmap> pix_list1;
	static QValueList<QPixmap> pix_list2;
	static QValueList<QPixmap> pix_list3;
	static bool run_once = false;
	if( run_once == false){
		pix_list1 . append(  QPixmap::fromMimeSource("car_g.png")  );
		pix_list2 . append(  QPixmap::fromMimeSource("car_b.png")  );
		pix_list2 . append(  QPixmap::fromMimeSource("car_r.png")  );
		run_once = true;
	}
	static QCanvasPixmapArray car_logo_g( pix_list1 );
	static QCanvasPixmapArray car_logo_b( pix_list2 );
	static QCanvasPixmapArray car_logo_r( pix_list3 );
	*/
	/*
	static QCanvasPixmapArray car_logo_g;
	static QCanvasPixmapArray car_logo_b("car_b.png");
	static QCanvasPixmapArray car_logo_r("car_r.png");
	static QCanvasPixmap pix_car_logo_g(QImage::fromMimeSource("car_g.png"));
	static bool run_once = false;
	if( run_once == false){
		car_logo_g . setImage(0, &pix_car_logo_g );
		run_once = true;
	}*/
	
	i = (rand()%(3-1+1))+1;
	_driver_behavior = DRIVER_BEHAVIOR(i);
	//debug("_driver_behavior = %d \n ", i);

	if(_driver_behavior == AGGRESSIVE)
		_acc_level = ACC;
	else
		_acc_level = CAR_ACC_LEVEL((rand()%(2-1+1))+1);

	if(_driver_behavior == CONSERVATIVE)
	    //setSequence(&car_logo_g);
	    setSequence(&car_logo_g);	// 設定車子的顏色為綠色
	else if(_driver_behavior == NORMAL)
	    setSequence(&car_logo_b);	// 設定車子的顏色為藍色
	else if(_driver_behavior == AGGRESSIVE)
	    setSequence(&car_logo_r);	// 設定車子的顏色為紅色
        
	setAnimated(FALSE);
}

double Car::realX()
{
	return _real_x;
}

double Car::realY()
{
	return _real_y;
}

void Car::setRealX(double p)
{
	_real_x = p;
}
void Car::setRealY(double p)
{
	_real_y = p;
}

/*
unsigned int Car::realVelocity()
{
	return _real_velocity ;
}

void Car::setRealVelocity(unsigned int v)
{
	if( v > _max_real_velocity)	return;
	//if( v < 0)	return; v is unsigned
	_real_velocity = v;
}*/
double Car::realVelocity()
{
	return _real_velocity; // use for count
}

void Car::setRealVelocity(double v)
{
	if( v > _max_real_velocity)	return;
	_real_velocity = v;
}


unsigned int Car::maxRealVelocity()
{
	return _max_real_velocity ;
}

void Car::setMaxRealVelocity(unsigned int v)
{
	_max_real_velocity = v;
}

unsigned int Car::minRealVelocity()
{
	return _min_real_velocity ;
}

void Car::setMinRealVelocity(unsigned int v)
{
	_min_real_velocity = v;
}

unsigned int Car::DriverBehavior()
{
	return _driver_behavior ;
}
void Car::setDriverBehavior(unsigned int i)
{
	/*
	static QCanvasPixmapArray car_logo_g("car_g.png");
	static QCanvasPixmapArray car_logo_b("car_b.png");
	static QCanvasPixmapArray car_logo_r("car_r.png");
        static QCanvasPixmapArray car_logo_y("car_y.png");
	*/
        
	_driver_behavior = DRIVER_BEHAVIOR(i);  // 1:Conservative  2:Normal 3:Aggressive 0.BCtrl_Mode
	
	if(_driver_behavior == BCTRL_MODE)
	    setSequence(&car_logo_y);
	else if(_driver_behavior == CONSERVATIVE)
	    setSequence(&car_logo_g);
	else if(_driver_behavior == NORMAL)
	    setSequence(&car_logo_b);
	else if(_driver_behavior == AGGRESSIVE)
	    setSequence(&car_logo_r);

    setAnimated(FALSE);
}

unsigned int Car::AccLevel()
{
	return _acc_level ;
}
void Car::setAccLevel(unsigned int i)
{
	_acc_level = CAR_ACC_LEVEL(i); 
}
void Car::draw( QPainter & p )
{
	QCanvasSprite ::draw( p );
	QColor color = red;

	color = Bulb2Color(frontBulb());
	if( _front_bulb != BULB_NONE ){
		p.setBrush(color);
		p.setPen(color);
		p.drawEllipse((int) x()-10,(int) y()-10+height() , 10,10);
	}

	color = Bulb2Color(backBulb());
	if( _back_bulb != BULB_NONE ){
		p.setBrush(color);
		p.setPen(color);
		p.drawEllipse((int) x()+width(),(int) y()-10+height() , 10,10);
	}

	if( _sel ){
		color = red;
		p.setBrush(color);
		p.setPen(color);
		p.drawLine( (int) x(),(int) y() , (int) x()+40,(int) y()); 
		p.drawLine( (int) x(),(int) y() , (int) x(),(int) y()+27); 
		p.drawLine( (int) x()+40,(int) y() , (int) x()+40,(int) y()+27); 
		p.drawLine( (int) x(),(int) y()+27 , (int) x()+40,(int) y()+27); 
	}
	
	 p.setBrush(black);
	 p.setPen(black);
	 p.drawText((int)x(),(int)y(),QString::number(_node_id));
	 p.drawText((int)x()+width(),(int)y(),QString::number(_real_velocity,'f',1)); 
}

void Car::setReLocation(bool b)
{
    _relocation = b;
}

void Car::setLan(int i)
{
    _lan = i;
    setAnimated(FALSE);
}

void Car::setSel(bool b)
{
    _sel = b;
    setAnimated(FALSE);
}

void Car::setChange2RightLan(bool b)
{
	/*
	if(_change2R == false && b == true )
		_last_time_change_lane = g_timer_ticks;
		*/
	_change2R = b;
	setAnimated(FALSE);
	_last_time_change_lane = g_timer_ticks;
}

void Car::setChange2LeftLan(bool b)
{
	/*
	if(_change2L == false && b == true )
		_last_time_change_lane = g_timer_ticks;
		*/
	_change2L = b;
	setAnimated(FALSE);
	_last_time_change_lane = g_timer_ticks;
}

void Car :: calNextVelocity(double time_gap)
{
	switch( AccLevel() ){
		case ACC_FULL: 
			setRealVelocity((realVelocity() + 4*time_gap));
			if( realVelocity() < minRealVelocity() ){
				setRealVelocity((realVelocity() + 10*time_gap));
			}
			break;
		case ACC: 
			setRealVelocity((realVelocity() + 1*time_gap));
			if( DriverBehavior() == AGGRESSIVE )
				setRealVelocity((realVelocity() + 2*time_gap)); 
			break;
		case DEC: 
			if( realVelocity() > minRealVelocity() )
				setRealVelocity((realVelocity() - 1*time_gap));
			else
				setRealVelocity((realVelocity() - 0.1*time_gap));
			break;
		case BRAKE_NORMAL:
			setRealVelocity((realVelocity() - (gFrictionFactorNormal*0.001*9.8)*time_gap));
			break;
		case BRAKE_EMERG:
			setRealVelocity((realVelocity() - (gFrictionFactorEmerg*0.001*9.8)*time_gap));
			break;
		case ACC_REL:     
		default:
			break;
	};

	if( realVelocity() < 0 )
		setRealVelocity(0);

}

bool Car::GetCollision()
{
	bool tmp = _collision;
	_collision = false;
	return tmp;
}
void Car::SetCollision()
{	
	_collision = true;	
};
bool Car::isCollision()
{
	return _collision;
}

