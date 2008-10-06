
#include <qtimer.h>

#include "Car.h"
#include "Road.h"
#include "global.h" 
#include "Coordinator.h"

Coordinator::Coordinator(QCanvas& c, QObject* parent , const char* name)
	: QObject(parent , name),
	  canvas(c),
	  _car_list(),
	  _timer( this, "Coordinator Timer"),
	  _select_item(NULL)
{
	connect( &_timer, SIGNAL( timeout() ), this, SLOT( active() ) );

//	_car_list . setAutoDelete( false );
	_center_car = NULL;
	_road = NULL;
	g_timer_ticks = 0;

	fixed_center = true;
	center_pos = 0;
}

void Coordinator::add2Coordinator(QCanvasItem* item)
{
	_sort_car_array[ _car_list . count() ] = (Car*)item;
	_car_list . append(item);

	if( !_center_car ){
		_center_car = (Car*) item;
		
		if(_road)
			_road->slotCenterCarChange(_center_car);
	}

	RealSortCarByX();
}

void Coordinator::Try2MoveCenterCar()
{
	if(!_center_car) return;
//	debug("in Try2MoveCenterCar");
	int wanna_cx = canvas.width()/2;
	int wanna_cy = canvas.height()/2-10;

	if(!fixed_center ){//計算要依據 center_ofset 改變wanna_cx

		//int tmp = center_pos - (_center_car -> realX() - gEndPosition*1000*10);
		int tmp = (_center_car -> realX() - gEndPosition*1000*10)  - center_pos;
		wanna_cx += tmp;
		debug("wanna_cx = %d, tmp = %d, realX=%lf, EndPos =%d, center_pos = %d",wanna_cx,tmp,_center_car->realX(),gEndPosition*1000*10,center_pos);
		_center_car -> setX( wanna_cx );

	} 
	_center_car -> setX( wanna_cx );
	_center_car -> setY( wanna_cy );
	
/*
	if(_center_car -> x() != wanna_cx  ){
		if( _center_car -> x() > wanna_cx ){
			_center_car -> setX( _center_car -> x() -1*gTimeRatio );
		}else{
			_center_car -> setX( _center_car -> x() +1*gTimeRatio ); 
		}
	}

	if(_center_car -> y() != wanna_cy ){
		if( _center_car -> y() > wanna_cy ){
			_center_car -> setY( _center_car -> y() -1*gTimeRatio);
		}else{
			_center_car -> setY( _center_car -> y() +1*gTimeRatio); 
		}
	}
	else
		gCollisionDetection = true;
		*/
}

void Coordinator::Try2MoveOtherCar(Car* car)
{
	double gap_x = _center_car -> realX() - car -> realX();
	double gap_y = _center_car -> realY() - car -> realY();

	car -> setX( _center_car -> x() - gap_x );
	car -> setY( _center_car -> y() - gap_y );
}

void Coordinator::clearCarsFlags() // clear car's flags , sel
{
	for (QCanvasItemList::Iterator it=_car_list.begin(); it!=_car_list.end(); ++it) {
		QCanvasItem *n = *it;
		if ( n ->rtti()==LOGO_RTTI) {
			// do something
		} else if ( n ->rtti()==CAR_RTTI) {
			Car* c = (Car*) n;
			c -> setSel(  false );
		} 
	}

}

void Coordinator::ShellSortCarByX()
{
	unsigned int car_count = _car_list.count();
	for(unsigned int i = 1 ; i < car_count ; i ++){
		unsigned int look_for = 5;			//每一次要往前看幾個
		if( i > 10 ) look_for = 5;
		else if( i > 5) look_for = 3;

		for(unsigned int ii = i - look_for ; ii < i ; ii ++){
			if( _sort_car_array[ii] -> realX() > _sort_car_array[i] -> realX() ){
				Car* car_tmp =  _sort_car_array[ii];
				 _sort_car_array[ii] =  _sort_car_array[i];
				 _sort_car_array[i] = car_tmp;
			} 
		} 
	}

	//以下是檢查.... 
	for(unsigned int i = 1 ; i < car_count ; i ++){
		if(_sort_car_array[i]->realX() < _sort_car_array[i-1]->realX() ){
			debug("Error::Car Array Sort Unsucess!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
			break;
		}
	} 
}

void Coordinator::RealSortCarByX()
{
	unsigned int car_count = _car_list.count();
	for(unsigned int i = 1 ; i < car_count ; i ++){
		for(unsigned int ii = 0 ; ii < i ; ii ++){
			if( _sort_car_array[ii] -> realX() > _sort_car_array[i] -> realX() ){
				Car* car_tmp =  _sort_car_array[ii];
				 _sort_car_array[ii] =  _sort_car_array[i];
				 _sort_car_array[i] = car_tmp;
			} 
		} 
	}

}

void Coordinator::CountCarsVelocity()
{
	
	for (QCanvasItemList::Iterator it=_car_list.begin(); it!=_car_list.end(); ++it) {
		QCanvasItem *n = *it;
		if ( n ->rtti()==LOGO_RTTI) 
		{
			// do something
		} 
		else if ( n ->rtti()==CAR_RTTI) 
		{
			Car* c = (Car*) n;

			if(c -> AccLevel() == 1)
			{
				c -> setCRealVelocity((c -> crealVelocity() + 1*gTimeRatio * 0.03));
				c -> setRealVelocity(int(c -> crealVelocity()));
			}
			else if(c -> AccLevel() == 3)
			{
				c -> setCRealVelocity((c -> crealVelocity() - 1*gTimeRatio * 0.03));
				c -> setRealVelocity(int(c -> crealVelocity()));
			}
			else  if(c -> AccLevel() == 4)
			{
				c -> setCRealVelocity((c -> crealVelocity() - gFrictionFactor*0.001*9.8*gTimeRatio * 0.03));
				c -> setRealVelocity(int(c -> crealVelocity()));
			}
		}
	}
}

void Coordinator::MoveCarsPosition()
{
	// change Position in the "Real World"
	for (QCanvasItemList::Iterator it=_car_list.begin(); it!=_car_list.end(); ++it) {
		QCanvasItem *n = *it;
		if ( n ->rtti()==LOGO_RTTI) {
			// do something
		} 
		else if ( n ->rtti()==CAR_RTTI) 
		{
			Car* c = (Car*) n;

			// Change Position by Velocity
			double gap = 0;

			//    ( Velocity * meter * bit_rate * time_ratio) * 0.030 msec / ( m * s) 
			gap = ( c -> realVelocity() * 1000.0 * 10 * gTimeRatio) * 0.03/ ( 60 * 60);

			c -> setRealX(  c -> realX() - gap );

			if(! (_center_car == c))
				c -> setX( c->realX() );
			
			if (c -> change2LeftLan())
			{
				if (c -> lan() == 1)
				{
	 				c -> setRealY(  c -> realY() + 1*gTimeRatio );
	 				if (c -> realY() == 148)
					{
						c -> setLan(2);
						c -> setChange2LeftLan(false);
					}
				}
				
				if (c -> lan() == 2)
				{
					if (c -> realY() < 148) //先按了右再按左
					{
						c -> setRealY(  c -> realY() + 1*gTimeRatio );
						if (c -> realY() == 148)
							c -> setChange2LeftLan(false);
					}
					else 
					{
						c -> setRealY(  c -> realY() + 1*gTimeRatio  );
		 				if (c -> realY() == 196)
						{
							c -> setLan(3);
							c -> setChange2LeftLan(false);
						}
					}
				}
				if (c -> lan() == 3)
				{
					if (c -> realY() < 196) //先按了右再按左
					{
						c -> setRealY(  c -> realY() + 1*gTimeRatio  );
						if (c -> realY() == 196)
							c -> setChange2LeftLan(false);
					}
				}
			}
			else if (c -> change2RightLan())
			{
				if (c -> lan() == 1)
				{
					if (c -> realY() > 100) //先按了左再按右
					{
						c -> setRealY(  c -> realY() - 1*gTimeRatio  );
						if (c -> realY() == 100)
							c -> setChange2RightLan(false);
					}
				}
				
				if (c -> lan() == 2)
				{
					if (c -> realY() > 148) //先按了左再按右
					{
						c -> setRealY(  c -> realY() - 1*gTimeRatio  );
						if (c -> realY() == 148)
							c -> setChange2RightLan(false);
					}
					else 
					{
		 				c -> setRealY(  c -> realY() - 1*gTimeRatio );
		 				if (c -> realY() == 100)
						{
							c -> setLan(1);
							c -> setChange2RightLan(false);
						}
					}
				}

				if (c -> lan() == 3)
				{
					c -> setRealY(  c -> realY() - 1*gTimeRatio  );
	 				if (c -> realY() == 148)
					{
						c -> setLan(2);
						c -> setChange2RightLan(false);
					}
				}
			}
		} 
	}

	// change Position in the "Canvas"
	if( !_select_item ){
		Try2MoveCenterCar();
	}

	for (QCanvasItemList::Iterator it=_car_list.begin(); it!=_car_list.end(); ++it) {
		QCanvasItem *n = *it;
		if( n == _select_item)	continue; //dlg setting

		if ( n ->rtti()==LOGO_RTTI) {
			// do something
		} else if ( n ->rtti()==CAR_RTTI) {
			Car* c = (Car*) n;
			if( c == _center_car ) continue;

			Try2MoveOtherCar(c);
		} 
	} 
}

static inline double DoubleAbs(double b)
{
//	return (b<0?-b:b);
	if( b < 0) return -b;
	else	return b;
}

static inline void BulbTestSet(Car::CAR_BULB &bulb, Car::CAR_BULB b)
{
	if(b > bulb)
		bulb = b;
}


void Coordinator::UpdateAroundCarArray()
{
	unsigned int car_count = _car_list.count();
	for(unsigned int i = 1 ; i < car_count ; i ++)
	{
		unsigned int look_for = 25;	//   100m (distance) / 4m (Car Length)
		unsigned int cf = 1;

		//右前方
		for(cf = 1 ; cf <= look_for ; cf ++)
		{
			if( (_sort_car_array[i] -> realY() - _sort_car_array[i-cf] -> realY()) > 1*CAR_HEIGHT && (_sort_car_array[i] -> realX() - _sort_car_array[i-cf] -> realX()) > 1*CAR_WIDTH )  
			{
				_sort_car_array[i] -> _around_car_array[0][0] = _sort_car_array[i-cf];
				break;
			}
			else
				_sort_car_array[i] -> _around_car_array[0][0] = NULL;
		}
		
		//右方
		for(cf = 1 ; cf <= 2 ; cf ++)
		{
			if( ( _sort_car_array[i] -> realY() - _sort_car_array[i-cf] -> realY()) > 1*CAR_HEIGHT && DoubleAbs( _sort_car_array[i-cf] -> realX() - _sort_car_array[i] -> realX()) < 1*CAR_WIDTH ) 
			{ 
				_sort_car_array[i] -> _around_car_array[0][1] = _sort_car_array[i-cf];
				break;
			}
			else if( ( _sort_car_array[i] -> realY() - _sort_car_array[i+cf] -> realY()) > 1*CAR_HEIGHT && DoubleAbs( _sort_car_array[i+cf] -> realX() - _sort_car_array[i] -> realX()) < 1*CAR_WIDTH ) 
			{ 
				_sort_car_array[i] -> _around_car_array[0][1] = _sort_car_array[i+cf];
				break;
			}
			else
				_sort_car_array[i] -> _around_car_array[0][1] = NULL;
		}			
		
		//右後方
		for(cf = 1 ; cf <= look_for ; cf ++)
		{
			if( (_sort_car_array[i] -> realY() - _sort_car_array[i+cf] -> realY()) > 1*CAR_HEIGHT && (_sort_car_array[i+cf] -> realX() - _sort_car_array[i] -> realX()) > 1*CAR_WIDTH )  
			{
				_sort_car_array[i] -> _around_car_array[0][2] = _sort_car_array[i+cf];
				break;
			}
			else
				_sort_car_array[i] -> _around_car_array[0][2] = NULL;
		}
		
		//正前方
		for(cf = 1 ; cf <= look_for ; cf ++)
		{
			if( DoubleAbs(_sort_car_array[i] -> realY() - _sort_car_array[i-cf] -> realY()) < 1*CAR_HEIGHT &&  (_sort_car_array[i] -> realX() - _sort_car_array[i-cf] -> realX()) < 1000 &&  (_sort_car_array[i] -> realX() > _sort_car_array[i-cf] -> realX()))    
			{
				_sort_car_array[i] -> _around_car_array[1][0] = _sort_car_array[i-cf];
				break;
			}
			else
				_sort_car_array[i] -> _around_car_array[1][0] = NULL;
		}
		
		//正後方
		for(cf = 1 ; cf <= look_for ; cf ++)
		{
			if( DoubleAbs(_sort_car_array[i] -> realY() - _sort_car_array[i+cf] -> realY()) < 1*CAR_HEIGHT &&  (_sort_car_array[i+cf] -> realX() - _sort_car_array[i] -> realX()) < 1000 &&  (_sort_car_array[i] -> realX() < _sort_car_array[i+cf] -> realX()))    
			{
				_sort_car_array[i] -> _around_car_array[1][2] = _sort_car_array[i+cf];
				break;
			}
			else
				_sort_car_array[i] -> _around_car_array[1][2] = NULL;
		}
		
		//左前方
		for(cf = 1 ; cf <= look_for ; cf ++)
		{
			if( (_sort_car_array[i-cf] -> realY() - _sort_car_array[i] -> realY()) > 1*CAR_HEIGHT && (_sort_car_array[i] -> realX() - _sort_car_array[i-cf] -> realX()) > 1*CAR_WIDTH )  
			{
				_sort_car_array[i] -> _around_car_array[2][0] = _sort_car_array[i-cf];
				break;
			}
			else
				_sort_car_array[i] -> _around_car_array[2][0] = NULL;
		}
		
		//左方
		for(cf = 1 ; cf <= 2 ; cf ++)
		{
			if( ( _sort_car_array[i-cf] -> realY() - _sort_car_array[i] -> realY()) > 1*CAR_HEIGHT && DoubleAbs( _sort_car_array[i-cf] -> realX() - _sort_car_array[i] -> realX()) < 1*CAR_WIDTH ) 
			{ 
				_sort_car_array[i] -> _around_car_array[2][1] = _sort_car_array[i-cf];
				break;
			}
			else if( ( _sort_car_array[i+cf] -> realY() - _sort_car_array[i] -> realY()) > 1*CAR_HEIGHT && DoubleAbs( _sort_car_array[i+cf] -> realX() - _sort_car_array[i] -> realX()) < 1*CAR_WIDTH ) 
			{ 
				_sort_car_array[i] -> _around_car_array[2][1] = _sort_car_array[i+cf];
				break;
			}			
			else
				_sort_car_array[i] -> _around_car_array[2][1] = NULL;
		}
		
		//左後方
		for(cf = 1 ; cf <= look_for ; cf ++)
		{
			if( (_sort_car_array[i+cf] -> realY() - _sort_car_array[i] -> realY()) > 1*CAR_HEIGHT && (_sort_car_array[i+cf] -> realX() - _sort_car_array[i] -> realX()) > 1*CAR_WIDTH )  
			{
				_sort_car_array[i] -> _around_car_array[2][2] = _sort_car_array[i+cf];
				break;
			}
			else
				_sort_car_array[i] -> _around_car_array[2][2] = NULL;
		}
		
	}
}


void Coordinator::UpdateBackCarBulb()
{
	unsigned int car_count = _car_list.count();
	_sort_car_array[car_count-1]->setBackBulb(Car::BULB_NONE); 
	for(unsigned int i = 0 ; i < car_count-1 ; i ++)
	{
		Car::CAR_BULB bulb_save = Car::BULB_NONE;

		if (_sort_car_array[i]->_around_car_array[1][2] != NULL)
		{
			// 先算出一些常用的變數 
			// vb 後車速度
			// vf 前車速度
			// distance 兩車的距離 
			
			double vf = _sort_car_array[i] -> realVelocity();
			double vb = _sort_car_array[i] -> (_around_car_array[1][2])->realVelocity();
			double distance = DoubleAbs(_sort_car_array[i]->_around_car_array[1][2]->realX() - CAR_WIDTH - _sort_car_array[i]->realX());
				
			if (distance < 1000) //通訊範圍為100m -> 1000 bit
			{
				//distance 3
				if( (vb-vf)*gTr*10 + ((double(vb*vb))-(double(vf*vf)))*10 / (2*gFrictionFactor*0.001*9.8) > distance) 
				{ 

					BulbTestSet(bulb_save, Car::BULB_RED);
				}
				
				//distance 2
				else if( (vb-vf)*gTr*10 + ((double(vb*vb))-(double(vf*vf)))*10 / (2*gFrictionFactor*0.001*9.8) > (distance-gSafeDistance*10)) 
				{

					BulbTestSet(bulb_save, Car::BULB_ORANGE);
				}

				//distance 1
				else if((vb+10)*10/2.0 > (distance - gSafeDistance*10))
				{
					BulbTestSet(bulb_save, Car::BULB_BLUE);
	 			}
				
				else
					BulbTestSet(bulb_save, Car::BULB_GREEN);

			}//end of 100m conneciton distance
			else
				BulbTestSet(bulb_save, Car::BULB_NONE);
		}
		else
		{
		//	_sort_car_array[i]->setBackBulb(Car::BULB_NONE); 
			BulbTestSet(bulb_save, Car::BULB_NONE);
			continue;
		}
		_sort_car_array[i]->setBackBulb(bulb_save); 
	} 
}

void Coordinator::UpdateFrontCarBulb()
{
	unsigned int car_count = _car_list.count();

	_sort_car_array[0]->setFrontBulb(Car::BULB_NONE); 
	for(unsigned int i = 1 ; i < car_count ; i ++){
		unsigned int look_for = 1;			//每一次要往前看幾個
		if( i > 10 ) look_for = 5;
		else if( i > 5) look_for = 3;

		unsigned int cf = 1;
		bool find = false;
		Car::CAR_BULB bulb_save = Car::BULB_NONE;
		for(cf = 1 ; cf <= look_for ; cf ++){
			if( DoubleAbs( _sort_car_array[i-cf] -> realY() - _sort_car_array[i] -> realY()) 
				< 1.5*CAR_HEIGHT) //在可視範圍內
			{
				find = true;

				// 先算出一些常用的變數 
				// vb 後車速度
				// vf 前車速度
				// distance 兩車的距離 
				 
				double vb = _sort_car_array[i] -> realVelocity();
				double vf = _sort_car_array[i-cf] -> realVelocity();
				double distance = DoubleAbs(_sort_car_array[i]->realX()-CAR_WIDTH-
							_sort_car_array[i-cf]->realX());
				unsigned int a=1;
				
				//check the overlap of car body
				if (_sort_car_array[i] -> reLocation() == false)
				{
					_sort_car_array[i] -> setReLocation(true);

					if(distance < 50)		
					     _sort_car_array[i] -> setRealX( _sort_car_array[i] -> realX() + 60);
				}

				if (distance < 1000) //通訊範圍為100m -> 1000 bit
				{
				
				//distance 1
//				if((vb-vf)*gTr*10 +  ((double(gMaxVelocity*gMaxVelocity))-(double(vf*vf)))*10/(2*gFrictionFactor*0.001*9.8) > (distance - gSafeDistance*10)) 
				if((vb+10)*10/2.0 > (distance - gSafeDistance*10))		
				{
					BulbTestSet(bulb_save, Car::BULB_BLUE);
					
					if(_sort_car_array[i]->DriverBehavior() == 1)		
						_sort_car_array[i]->setAccLevel(3);		
					else if(_sort_car_array[i]->DriverBehavior() == 2)
						_sort_car_array[i]->setAccLevel(2);		
				}
				else
				{
					BulbTestSet(bulb_save, Car::BULB_NONE);
				 	
					if( ((vb+10)*10/2.0 < (distance + 5*gSafeDistance*10)) && (_sort_car_array[i]->DriverBehavior() == 3))
						_sort_car_array[i]->setAccLevel(1);
					
					if( ((vb+10)*10/2.0 < (distance + 10*gSafeDistance*10)) && (_sort_car_array[i]->DriverBehavior()  < 3))
					{
						a = (rand()%(2-1+1))+1;
						_sort_car_array[i]->setAccLevel(a);
					}
				}
				
				//distance 2
				if( (vb-vf) * gTr * 10 + ((double(vb*vb)) - (double(vf*vf))) * 10 / (2*gFrictionFactor*0.001*9.8) > distance - gSafeDistance*10) 
				{
					BulbTestSet(bulb_save, Car::BULB_ORANGE);
					
					if(_sort_car_array[i]->DriverBehavior() == 1)		
						_sort_car_array[i]->setAccLevel(4);		
					else if(_sort_car_array[i]->DriverBehavior() == 2)
						_sort_car_array[i]->setAccLevel(3);		
					else if(_sort_car_array[i]->DriverBehavior() == 3)
						_sort_car_array[i]->setAccLevel(2);		
				}
				
				//distance 3
				if( (vb-vf) * gTr * 10 + ((double(vb*vb)) - (double(vf*vf))) * 10 / (2*gFrictionFactor*0.001*9.8) > distance) 
				{ 
					BulbTestSet(bulb_save, Car::BULB_RED);
					
					if(_sort_car_array[i]->DriverBehavior() < 3)		
						_sort_car_array[i]->setAccLevel(4);		
					else if(_sort_car_array[i]->DriverBehavior() == 3)
						_sort_car_array[i]->setAccLevel(3);		
				}

				//distance 4
				if( (vb-vf) * gTr * 10 + ((double(vb*vb)) - (double(vf*vf))) * 10 / (2*gFrictionFactor*0.001*9.8) > distance + 400) //400 means car length (bit)
				{ 
					BulbTestSet(bulb_save, Car::BULB_RED);
					//every driver break
					_sort_car_array[i]->setAccLevel(4);		
				}
				break;
				
				}//end of 100m conneciton distance
				else
				{
					BulbTestSet(bulb_save, Car::BULB_NONE);
				 	
					if( ((vb+10)*10/2.0 < (distance + 5*gSafeDistance*10)) && (_sort_car_array[i]->DriverBehavior() == 3))
						_sort_car_array[i]->setAccLevel(1);
					
					if( ((vb+10)*10/2.0 < (distance + 10*gSafeDistance*10)) && (_sort_car_array[i]->DriverBehavior()  < 3))
					{
						a = (rand()%(2-1+1))+1;
						_sort_car_array[i]->setAccLevel(a);
					}
				}
			}
		}
		if( find == false){
			_sort_car_array[i]->setFrontBulb(Car::BULB_GREEN); 
			continue;
		}
		_sort_car_array[i]->setFrontBulb(bulb_save); 
	} 
}


void Coordinator::active()
{
	if( !_center_car ){
//		debug("maybe u should select some center car first");
		return;
	}
	g_timer_ticks += (1*gTimeRatio);
	emit sigUpdateEnable(false);

	CountCarsVelocity();
	MoveCarsPosition();

	//ShellSortCarByX();
	RealSortCarByX();

	UpdateAroundCarArray();
	UpdateFrontCarBulb();
	UpdateBackCarBulb();

	if(_road)	_road -> repaint();

	emit sigUpdateEnable(true);
}

void Coordinator::slotPlay()
{
//	_timer . start(gSystemHandleTime+gMachineWorkTime); // 60msec default now @@"
//	_timer . start(gSystemHandleTime); // 20msec default now @@"
	_timer . start(30); // 30msec default now @@"
}

void Coordinator::slotPause()
{
	_timer . stop();
}

void Coordinator::slotStop()
{
	_timer . stop(); 
}
