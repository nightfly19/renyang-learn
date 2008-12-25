
#include "Car.h"
#include "InternetCar.h"

static inline double Square(double d)
{
	return d*d;
}
InternetCar::InternetCar( QCanvas* canvas)
	: Car( canvas )
	, front_closest_speed(0)
	, smart_brake(true)
	, pos_list ()
{
	pos_list . setAutoDelete( true );	
}

void InternetCar :: UpdateWarnningLight(bool clear) 
{
//	debug("UdateWarnningLight @@" );
	CAR_BULB save_front_bulb = BULB_NONE;
	CAR_BULB save_back_bulb = BULB_NONE;
	if( !clear ){
		save_front_bulb = frontBulb();
		save_back_bulb = backBulb();
	}
	front_closest_speed = gMaxVelocity;

	double save_dis = (realVelocity() / 2)*10;
	for(InfoElement * p = pos_list.first() ; p ; p = pos_list.next() ){
		if( DoubleAbs( p -> SndrY() - p -> RecvY())
				> 1.5*CAR_HEIGHT) //在可視範圍外
			continue;
		BulbTestSet(save_front_bulb, BULB_GREEN);

		double DD = p->SndrX() - p->RecvX(); // 此封包傳送時的位置與接收時的位置相減
		double Dns =10* (Square( KMHR_2_MS(p->SndrSpeed()) ) / (2*gFrictionFactorNormal/1000.0*9.8));
		double Dnr =10* (Square( KMHR_2_MS(realVelocity()) ) / (2*gFrictionFactorNormal/1000.0*9.8))
			+ KMHR_2_MS(realVelocity())*10.0*(gHumanResponseTime+gSystemHandleTime)/1000.0;
		double Dnw =  DoubleAbs(Dns-Dnr)+20;

		double Des =10* (Square( KMHR_2_MS(p->SndrSpeed()) ) / (2*gFrictionFactorEmerg/1000.0*9.8));
		double Der =10* (Square( KMHR_2_MS(realVelocity()) ) / (2*gFrictionFactorEmerg/1000.0*9.8))
			+ KMHR_2_MS(realVelocity())*10.0*(gHumanResponseTime+gSystemHandleTime)/1000.0;
		double Dew =  DoubleAbs(Des-Der)+20;


//		debug("Dns,Des (%f,%f) Dnr,Der (%f,%f) -- Dnw,Dew (%f,%f)",Dns,Des,Dnr,Der,Dnw,Dew);
		//if(DD <= CAR_WIDTH){//前燈
		if(DD <= 0){//前燈
			if(DoubleAbs(DD) < save_dis)
				BulbTestSet(save_front_bulb, BULB_BLUE);
			if(DoubleAbs(DD) < Dnw ){
				BulbTestSet(save_front_bulb, BULB_ORANGE);
			}
			if(DoubleAbs(DD) < Dew ){
				BulbTestSet(save_front_bulb, BULB_RED);
				if(front_closest_speed > p->SndrSpeed() ){
					front_closest_speed = p->SndrSpeed();
				}
			}
		}
		if(DD >= -CAR_WIDTH ){//後燈
			if(DoubleAbs(DD) < save_dis)
				BulbTestSet(save_back_bulb, BULB_BLUE);
			if(DoubleAbs(DD) < Dnw )
				BulbTestSet(save_back_bulb, BULB_ORANGE);
			if(DoubleAbs(DD) < Dew )
				BulbTestSet(save_back_bulb, BULB_RED);
		}
		if(DoubleAbs(DD) < CAR_WIDTH){ // 由接收到的封包去判斷是否有發生碰撞
			SetCollision();
		}

	} 
	setFrontBulb( save_front_bulb );
	setBackBulb( save_back_bulb );
	return;
}

void InternetCar :: TestLeftAndRight( bool& lns, bool& rns)
{
	for(InfoElement * p = pos_list . first() ; p ; p = pos_list . next() ){
		if( DoubleAbs( p->SndrX() - p->RecvX() ) < ((gSafeDistance*10) * 8) ){ //四倍安全車距
			if( p->SndrY() != p->RecvY() && (DoubleAbs(  p->SndrY() - p->RecvY() ) <= 48.0) ){ //在一個車道之內
				if(p->RecvY() > p->SndrY() )
					rns = true;
				else if(p->RecvY() < p->SndrY() )
					lns = true;

			}
		}
	} 
}

void InternetCar :: Try2ChangeLane()
{
	//debug("WifiCar :: Try2ChangeLane() ");
	bool wanna_change_lane =false;
	if( g_timer_ticks - LastTimeChangeLane()  < 2000 )
		return;
	if( change2LeftLan() || change2RightLan() || DriverBehavior() == BCTRL_MODE || isCollision() )
		return;
	switch( frontBulb() ){
		case BULB_RED:
			wanna_change_lane = true;
			break;
		case BULB_ORANGE:
			if( DriverBehavior() == AGGRESSIVE )
				wanna_change_lane = true;
				/*
			else if( DriverBehavior() == NORMAL)
				wanna_change_lane = true;
				*/
			break;
		default:
			break; 
	}
	if(  DoubleAbs(realVelocity() - maxRealVelocity()) < 3.0 )
		return;

	if( realVelocity() < minRealVelocity() ) wanna_change_lane = true;
	if( wanna_change_lane == false ) return;

	bool left_not_save = false;
	bool right_not_save = false;
	TestLeftAndRight( left_not_save , right_not_save);
	switch( lan() ){
		case 1: //100
			if(!left_not_save){
				setChange2LeftLan(true);
			}
			break;
		case 2: //148
			if(!left_not_save){
				setChange2LeftLan(true);
			}else if(!right_not_save){
				setChange2RightLan(true);
			}
			break;
		case 3: //196
			if(!right_not_save){
				setChange2RightLan(true);
			}
			break; 
	}
	
/*
	if( change2LeftLan() || change2RightLan() )
		debug("try to change lane");
	else
		debug("do not change lane");
*/
}



void InternetCar::draw( QPainter & p )
{
	Car::draw( p );

}

void InternetCar::UpdateCarByWarnning()
{
	switch( frontBulb()){
		case BULB_BLUE:
			if(DriverBehavior() == CONSERVATIVE)
				setAccLevel(ACC_REL);
			else if(DriverBehavior() == NORMAL)
				setAccLevel(ACC);
			else if(DriverBehavior() == AGGRESSIVE)
				setAccLevel(ACC_FULL);
			break;
		case BULB_ORANGE:
			if(DriverBehavior() ==  CONSERVATIVE)
				setAccLevel(BRAKE_NORMAL);
			else if(DriverBehavior() == NORMAL)
				setAccLevel(BRAKE_NORMAL);
			else if(DriverBehavior() == AGGRESSIVE)
				setAccLevel(DEC);
			break;
		case BULB_RED:
			setAccLevel(BRAKE_EMERG);
			break;
		case BULB_YELLOW:
			setAccLevel(BRAKE_NORMAL);
			break;
		case BULB_NONE:
		case BULB_GREEN:
		default:
			setAccLevel(ACC_FULL);
			break;
	}
}

void InternetCar::calNextVelocity(double time_gap)
{
	Car::calNextVelocity(time_gap);
	if(SmartBrake() && AccLevel() == BRAKE_EMERG ){
		if(realVelocity() >  front_closest_speed ){
			setRealVelocity( front_closest_speed );
			setAccLevel(DEC);
		} 
	}
}

