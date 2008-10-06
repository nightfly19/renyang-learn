
#include <qtimer.h>

#include "Car.h"
#include "WifiCar.h"
#include "DSRCCar.h"
#include "Road.h"
#include "global.h" 
#include "Coordinator.h"

Coordinator::Coordinator(QCanvas& c, QObject* parent , const char* name)
	: QObject(parent , name),
	  canvas(c),
	  _car_list(),
	  _car_timer( this, "Car Coordinator Timer"),
	  _gps_timer( this, "GPS Timer"),
	  _ticks_timer( this, "Ticks Timer"),
	  _select_item(NULL)
{
	connect( &_car_timer, SIGNAL( timeout() ), this, SLOT( car_active() ) );
	connect( &_gps_timer, SIGNAL( timeout() ), this, SLOT( gps_active() ) );
	connect( &_ticks_timer, SIGNAL( timeout() ), this, SLOT( ticks_active() ) );

//	_car_list . setAutoDelete( false );
	_center_car = NULL;
	_road = NULL;
	g_timer_ticks = 0;

	fixed_center = true;
	center_pos = 0;
	_lock_try_active_gps_broadcast = false;
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
	double wanna_cx = canvas.width()/2;
	double wanna_cy = canvas.height()/2-10;

	if( _center_car -> realX() < gEndPosition*1000*10 - 100 ){
		_center_car -> setRealX(gStartPosition*1000*10);	
	}
	if(!fixed_center ){//計算要依據 center_ofset 改變wanna_cx

		//int tmp = center_pos - (_center_car -> realX() - gEndPosition*1000*10);
		int tmp =(int) (_center_car -> realX() - gEndPosition*1000*10)  - center_pos;
		wanna_cx += tmp;
//		debug("wanna_cx = %d, tmp = %d, realX=%lf, EndPos =%d, center_pos = %d",wanna_cx,tmp,_center_car->realX(),gEndPosition*1000*10,center_pos);
		_center_car -> setX( wanna_cx );
	} 
	if(_center_car -> realY() != 148){ //TODO: 這樣是不對的哦
		wanna_cy += (_center_car->realY()-148.0);
	}
	_center_car -> setX( wanna_cx );
	_center_car -> setY( wanna_cy );
	
}

void Coordinator::Try2MoveOtherCar(Car* car)
{
	if( car -> realX() < gEndPosition*1000*10 - 100 ){
		car -> setRealX(gStartPosition*1000*10);	
	}

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
	/*
	for(unsigned int i = 1 ; i < car_count ; i ++){
		if(_sort_car_array[i]->realX() < _sort_car_array[i-1]->realX() ){
			debug("Error::Car Array Sort Unsucess!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
			break;
		}
	} */
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

void Coordinator::CountCarsVelocity(double timer_gap)
{
	
	for (QCanvasItemList::Iterator it=_car_list.begin(); it!=_car_list.end(); ++it) {
		QCanvasItem *n = *it;
		if ( n ->rtti()==LOGO_RTTI) {
			// do something
			continue;
		} 
		else if ( n ->rtti()==CAR_RTTI) {
			Car* c = (Car*) n;
			if(c && c->DriverBehavior()!=BCTRL_MODE 
				&& c->AccLevel()!=ACTRL_MODE)
				c->calNextVelocity( gTimeRatio * (timer_gap/1000.0) );
				/*
			if(c -> AccLevel() == ACC) {
				c -> setRealVelocity((c -> realVelocity() + 1*gTimeRatio * (g_car_timer_gap/1000.0)));
			}
			else if(c -> AccLevel() == DEC) {
				c -> setRealVelocity((c -> realVelocity() - 1*gTimeRatio * (g_car_timer_gap/1000.0)));
			}
			else  if(c -> AccLevel() == BRAKE_NORMAL) {
				c -> setRealVelocity((c -> realVelocity() - gFrictionFactorNormal*0.001*9.8*gTimeRatio * (g_car_timer_gap/1000.0)));
			} else  if(c -> AccLevel() == BRAKE_EMERG) {
				c -> setRealVelocity((c -> realVelocity() - gFrictionFactorEmerg*0.001*9.8*gTimeRatio * (g_car_timer_gap/1000.0)));
			}
			*/
		}
	}
}

void Coordinator::MoveCarsPosition(double timer_gap)
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
			//gap = ( c -> realVelocity() * 1000.0 * 10 * gTimeRatio) * 0.03/ ( 60 * 60);
			gap = ( c -> realVelocity() * 1000.0 * 10 * gTimeRatio) * ((timer_gap)/1000.0) / ( 60 * 60);

			c -> setRealX(  c -> realX() - gap );

			if(! (_center_car == c))
				c -> setX( c->realX() );
			if (c -> change2LeftLan()) {
				if (c -> lan() == 1) {
	 				c -> setRealY(  c -> realY() + 1*gTimeRatio );
					if (int(c -> realY()) >= 148) {
						c -> setRealY( 148 );
						c -> setLan(2);
						c -> setChange2LeftLan(false);
					}
				}
				
				if (c -> lan() == 2) {
					if (int(c -> realY()) < 148){ //先按了右再按左
						c -> setRealY(  c -> realY() + 1.0*gTimeRatio );
						if (int(c -> realY())>= 148){
							c -> setRealY( 148 );
							c -> setChange2LeftLan(false);
						}
					}
					else {
						c -> setRealY(  c -> realY() + 1.0*gTimeRatio  );
		 				if (int(c -> realY()) >= 196)
						{
							c -> setRealY( 196 );
							c -> setLan(3);
							c -> setChange2LeftLan(false);
						}
					}
				}
				if (c -> lan() == 3) {
					if (c -> realY() < 196){ //先按了右再按左
						c -> setRealY(  c -> realY() + 1.0*gTimeRatio  );
						if (int(c -> realY()) >= 196){
							c -> setRealY( 196 );
							c -> setChange2LeftLan(false);
						}
					}
				}
			}
			else if (c -> change2RightLan()) {
				if (c -> lan() == 1) {
					if (c -> realY() > 100){ //先按了左再按右
						c -> setRealY(  c -> realY() - 1.0*gTimeRatio  );
						if (int(c -> realY()) <= 100){
							c -> setRealY( 100 );
							c -> setChange2RightLan(false);
						}
					}else if( c -> realY() < 100){
						c -> setRealY( 100 );
						c -> setChange2RightLan(false);
					}
				}
				
				if (c -> lan() == 2) {
					if (c -> realY() > 148) { //先按了左再按右
						c -> setRealY(  c -> realY() - 1.0*gTimeRatio  );
						if (int(c -> realY()) == 148){
							c -> setRealY(148);
							c -> setChange2RightLan(false);
						}
					}
					else {
		 				c -> setRealY(  c -> realY() - 1.0*gTimeRatio );
		 				if (int(c -> realY()) <= 100) {
							c -> setRealY(100);
							c -> setLan(1);
							c -> setChange2RightLan(false);
						}
					}
				}

				if (c -> lan() == 3) {
					c -> setRealY(  c -> realY() - 1.0*gTimeRatio  );
	 				if (int(c -> realY()) == 148) {
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
void Coordinator::UpdateAroundCarArray()
{
/*
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
	*/
}


void Coordinator::UpdateBackCarBulb()
{
	unsigned int car_count = _car_list.count();
	_sort_car_array[car_count-1]->setBackBulb(BULB_NONE); 
	for(unsigned int i = 0 ; i < car_count-1 ; i ++){
		unsigned int look_for = 5;			//每一次要往後看幾個
		if( car_count - i < 10 ) look_for = (car_count - i) - 1;

		unsigned int cf = 1;
		bool find = false;
		CAR_BULB bulb_save = BULB_NONE;
		for(cf = 1 ; cf <= look_for ; cf ++){
//			if( _sort_car_array[i+cf] -> realY() == _sort_car_array[i] -> realY()) 
			if( DoubleAbs( _sort_car_array[i+cf] -> realY() - _sort_car_array[i] -> realY()) 
				< 1.5*CAR_HEIGHT) //在可視範圍內
			{
				find = true;

				// 先算出一些常用的變數 
				// vb 後車速度
				// vf 前車速度
				// distance 兩車的距離 

				double vf = _sort_car_array[i] -> realVelocity();
				double vb = _sort_car_array[i+cf] -> realVelocity();
				double distance = DoubleAbs(_sort_car_array[i+cf]->realX()-CAR_WIDTH-
						_sort_car_array[i]->realX());
				
				if (distance < 1000) //通訊範圍為100m -> 1000 bit
				{

					//distance 1
					//if(vb*10/2 +  ((double(vb*vb))-(double(vf*vf)))*10/(2*gFrictionFactor*0.001*9.8) > (distance - gSafeDistance*10)) 
					if((vb+10)*10/2.0 > (distance - gSafeDistance*10))
					{
						BulbTestSet(bulb_save, BULB_BLUE);
					}
					else
						BulbTestSet(bulb_save, BULB_GREEN);

					//distance 2
					if( (vb-vf)*gTr*10 + ((double(vb*vb))-(double(vf*vf)))*10 / (2*gFrictionFactorNormal*0.001*9.8) > 
							(distance-gSafeDistance*10)) {

						BulbTestSet(bulb_save, BULB_ORANGE);
					}
					if( (vb-vf)*gTr*10 + ((double(vb*vb))-(double(vf*vf)))*10 / (2*gFrictionFactorNormal*0.001*9.8) > distance) { 

						BulbTestSet(bulb_save, BULB_RED);
					}
					break;

				}//end of 100m conneciton distance
				else
					BulbTestSet(bulb_save, BULB_NONE);
			}
		}
		if( find == false){
			_sort_car_array[i]->setBackBulb(BULB_GREEN); 
			continue;
		}
		_sort_car_array[i]->setBackBulb(bulb_save); 
	} 
}

void Coordinator::UpdateFrontCarBulb()
{
	unsigned int car_count = _car_list.count();

	_sort_car_array[0]->setFrontBulb(BULB_NONE); 
	for(unsigned int i = 1 ; i < car_count ; i ++){
		unsigned int look_for = 1;			//每一次要往前看幾個
		if( i > 10 ) look_for = 5;
		else if( i > 5) look_for = 3;

		unsigned int cf = 1;
		bool find = false;
		CAR_BULB bulb_save = BULB_NONE;
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
				//unsigned int a=1;
				
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
						BulbTestSet(bulb_save, BULB_BLUE);
					}
					else
					{
						BulbTestSet(bulb_save, BULB_NONE);
					}

					//distance 2
					if( (vb-vf) * gTr * 10 + ((double(vb*vb)) - (double(vf*vf))) * 10 / (2*gFrictionFactorNormal*0.001*9.8) > distance - gSafeDistance*10) 
					{
						BulbTestSet(bulb_save, BULB_ORANGE);
					}

					//distance 3
					if( (vb-vf) * gTr * 10 + ((double(vb*vb)) - (double(vf*vf))) * 10 / (2*gFrictionFactorNormal*0.001*9.8) > distance) 
					{ 
						BulbTestSet(bulb_save, BULB_RED);

					}

					//distance 4
					if( (vb-vf) * gTr * 10 + ((double(vb*vb)) - (double(vf*vf))) * 10 / (2*gFrictionFactorNormal*0.001*9.8) > distance + 400) //400 means car length (bit)
					{ 
						BulbTestSet(bulb_save, BULB_RED);
					}
//					break;

				}//end of 100m conneciton distance
				else {
					BulbTestSet(bulb_save, BULB_NONE);
				}
			}
		}
		if( find == false){
			_sort_car_array[i]->setFrontBulb(BULB_GREEN); 
			continue;
		}
		_sort_car_array[i]->setFrontBulb(bulb_save); 
	} 
}

void Coordinator::slotPause()
{
	_car_timer . stop();
	_gps_timer . stop();
	_ticks_timer . stop();
}

void Coordinator::slotStop()
{
	_car_timer . stop(); 
	_gps_timer . stop(); 
	_ticks_timer . stop();
}
double Coordinator::CountCarVelocity()
{
	unsigned int car_count = _car_list.count();
	double total = 0;
	for(unsigned int i = 0 ; i < car_count ; i ++){
		//total += (double)(_sort_car_array[i]->realVelocity());
		total += (((Car*)_car_list[i])->realVelocity());
	}

	return total /(double) (car_count);
}
void Coordinator::UpdateWarnningSystem()
{
	/*
	unsigned int car_count = _car_list.count();

	static int ch_lane_gap = 0;
	for(unsigned int i = 0 ; i < car_count ; i ++){
		((Car*)_car_list[i] )-> UpdateCarByWarnning();
		if((i+ch_lane_gap)%8 == 0 && 
			 ( g_timer_ticks - ((Car*)_car_list[i])->LastTimeChangeLane() ) > 700 )
		{
			((Car*)_car_list[i]) -> Try2ChangeLane();
			ch_lane_gap = (ch_lane_gap+1)%8;
		}
	}
	*/

}
void Coordinator::UpdateCarBulb()
{
	unsigned int car_count = _car_list.count();
	for(unsigned int i = 0; i < car_count ; i ++){
		Car* car = (Car*) _car_list[i];
		WifiCar* wc= (WifiCar*) _car_list[i];
		DSRCCar* dc= (DSRCCar*) _car_list[i];
		if(car->CarType() == CT_WIFI)
			wc->UpdateWarnningLight();
		else if(car->CarType() == CT_DSRC)
			dc->UpdateWarnningLight();
	}
}

void Coordinator::car_active()
{
	static double last_modify_timer = g_timer_ticks;
	double timer_gap = g_timer_ticks - last_modify_timer;

	if( !_center_car ){
		return;
	}
	//g_timer_ticks += (1*gTimeRatio); //move to timer ticks
	emit sigUpdateEnable(false);

	CountCarsVelocity(timer_gap);
	MoveCarsPosition(timer_gap);

	//ShellSortCarByX();
	//RealSortCarByX();
	//UpdateFrontCarBulb();
	//UpdateBackCarBulb();
	
//	UpdateCarBulb();
//	UpdateWarnningSystem();

	if(_road)	_road -> repaint();

	emit sigUpdateEnable(true);

	last_modify_timer = g_timer_ticks;
}

void Coordinator :: gps_active()
{
	unsigned int car_count = _car_list.count();
	double last_time = 0;

	for(unsigned int i = 0; i < car_count ; i ++){
		Car* car = (Car*) _car_list[i];
		if( !car )	continue;
		last_time = car -> GPSUpdateTime();	
		if( last_time == 0 ){
			last_time = ((rand()%(300))+200) + g_timer_ticks;
			if( car-> AccLevel() == BRAKE_NORMAL){
				last_time = ((rand()%(100))+50) + g_timer_ticks;
			}
			if( car->change2RightLan() || car->change2LeftLan() ){
				last_time = ((rand()%(100))+50) + g_timer_ticks;
			}
			if( car-> AccLevel() == BRAKE_EMERG || car->backBulb() == BULB_RED ){
				last_time = ((rand()%(50))+20) + g_timer_ticks;
			}
			car ->  setGPSUpdateTime( last_time );
			if(car -> CarType() == CT_WIFI || car -> CarType() == CT_DSRC ){
				((InternetCar*)car) -> Try2ChangeLane();
			}
		}
	}
}

void Coordinator::ticks_active()
{
	//g_timer_ticks += (1*gTimeRatio*g_car_timer_gap);
	g_timer_ticks += (1*gTimeRatio*g_ticks_timer_gap);

	if( g_timer_ticks > g_simulate_until *1000 ){
		slotPause();

		emit simulationDone();
	}

	if( _lock_try_active_gps_broadcast )	return;
	_lock_try_active_gps_broadcast = true;
	/* check 是否有需要BrLocation的Car */
	double last_time = 0;
	unsigned int car_count = _car_list.count();
	for(unsigned int i = 0; i < car_count ; i ++){
		Car* car = (Car*) _car_list[i];
		last_time = car -> GPSUpdateTime();	
		if( last_time == 0 )	continue;
		WifiCar* wc = (WifiCar*) _car_list[i];
		DSRCCar* dc = (DSRCCar*) _car_list[i];

		switch( car -> CarType() ){
			case CT_WIFI:
				wc->UpdateWarnningLight(true);
				wc->UpdateCarByWarnning();
				if(  g_timer_ticks >  last_time ){
					if( wc -> BrLocation() )
						car -> setGPSUpdateTime(0);
					else
						car -> setGPSUpdateTime(g_timer_ticks+2);
				}
				break;
			case CT_DSRC:
				dc->UpdateWarnningLight(true);
				dc->UpdateCarByWarnning();
				if(  g_timer_ticks >  last_time ){
					if( dc -> BrLocation() )
						car -> setGPSUpdateTime(0);
					else
						car -> setGPSUpdateTime(g_timer_ticks+2);
				}
				break;
			default: break;
		} 
	} 
	_lock_try_active_gps_broadcast = false;

	return;
}

void Coordinator::slotPlay()
{
//	_timer . start(gSystemHandleTime+gMachineWorkTime); // 60msec default now @@"
//	_timer . start(gSystemHandleTime); // 20msec default now @@"
	_car_timer . start( g_car_timer_gap ); // 30msec default now @@"
	_gps_timer . start( g_gps_timer_gap ); // 300msec default now @@"
	_ticks_timer . start( g_ticks_timer_gap ); // 30msec default now @@"
}

int Coordinator :: CountCollision()
{
	unsigned int tmp_count = 0;
	unsigned int car_count = _car_list.count();
	for(unsigned int i = 0; i < car_count ; i ++){
		Car* car = (Car*) _car_list[i];
		if( car -> GetCollision() )
			tmp_count ++; 
	}
	g_car_collision += tmp_count;

	return tmp_count;
}
