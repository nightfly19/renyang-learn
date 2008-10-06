
#include <qpainter.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qlistbox.h>
#include "network/packet.h"
#include "Car.h"
#include "DSRCCar.h"
#include "network/dsrc_global.h"

DSRCCar::DSRCCar( QCanvas* canvas)
	//: Car( canvas)
	: InternetCar( canvas)
	, ni1( this )
	//, chchmode(CHCH_POLL)
	, chchmode(CHCH_ID_POLL)
{
	connect( &ni1, SIGNAL( sigPacketRecv(Packet*) ) , this , SLOT(dataRecv(Packet*)));
}

int DSRCCar :: ChannelPolling( )
{
	for(int ch = CH_NORMAL1; ch <= CH_NORMAL4; ch++){
		if(ni1 . isIdle(ch) ){
			return ch;
		}
	} 
	return -1;
}
int DSRCCar :: ChannelPollingMod4( )
{
	int mod = nodeID() % 4;
	for(int ch = CH_NORMAL1+mod; ch <= CH_NORMAL4+mod; ch++){
		if(ni1 . isIdle(ch%4) ){
			return ch%4;
		}
	} 
	return -1;
}
int DSRCCar :: ChannelWarnPolling()
{
	for(int ch = CH_WARNG1; ch <= CH_WARNG2; ch++){
		if(ni1 . isIdle(ch) ){
			return ch;
		}
	} 
	return ChannelPolling();
}
int DSRCCar :: ChannelWarnPollingMod2()
{
	switch( nodeID() %2 ){
		case 0:
			if(ni1 . isIdle( CH_WARNG1 ) )
				return CH_WARNG1;
			else if(ni1 . isIdle( CH_WARNG2 ) )
				return CH_WARNG2;
			break;
		case 1:
			if(ni1 . isIdle( CH_WARNG2 ) )
				return CH_WARNG2;
			else if(ni1 . isIdle( CH_WARNG1 ) )
				return CH_WARNG1;
			break;

	}
	return ChannelPollingMod4();
}

bool DSRCCar::BrLocation()
{
	int channel = -1;
	if( AccLevel() == BRAKE_EMERG || backBulb() == BULB_RED){
		if( ChChMode() == CHCH_POLL ){
			channel = ChannelWarnPolling();
		}else if( ChChMode() == CHCH_ID_POLL ){
			channel = ChannelWarnPollingMod2(); 
		}
	}else{
		if( ChChMode() == CHCH_POLL ){
			channel = ChannelPolling();
		}else if( ChChMode() == CHCH_ID_POLL ){
			channel = ChannelPollingMod4(); 
		}
	}

	if( channel == -1 )	return false;
	InfoPacket *p = new InfoPacket(  realX(), realY() , nodeID(), realVelocity() );

//	debug("DSRCCar BrLocation::  (%e,%e)",p->X(),p->Y());

	return ni1 . BrSend( p , channel);
}

void DSRCCar:: setNetCoor(NetCoordinator* nc)
{
	ni1 . setNetCoor(nc);

	nc -> addNI( &ni1 );
}
void DSRCCar::draw( QPainter & p )
{
	/*
	if( ni1 . isSending() ){
		p . setPen( blue );
		p . setBrush( blue );
		p . drawArc( int(x() -1000) , int(y()-1000) , 2000, 2000 , 5760 , 5760);
		p . drawArc( int(x() -50) , int(y()-50) , 100, 100 , 5760 , 5760);
	}else if( ni1 . isRecving() ){
		p . setPen( red );
		p . setBrush( red );
		p . drawPie( int(x() - 30)  , int(y() -30) , 60, 60 , 0 , 5760);
	}*/
	for(int i = 0 ; i < CH_MAX ; i ++){
		p . setPen( ChannelColors[ChDsrc2Number(i)] );
		p . setBrush( ChannelColors[ChDsrc2Number(i)] );
		switch ( ni1 . GetState( i ) ){
			case (Phy::PHY_SEND):
				p . drawArc( int(x() -1000)+i, int(y()-1000) , 2000, 2000 , 5760 , 5760);
				//p . drawArc( int(x() -50)+i, int(y()-50) , 100, 100 , 5760 , 5760); 
				p . drawPie( int(x() - 50)+i , int(y() -50) , 100, 100, 5760, 5760);
				break;
			case (Phy::PHY_RECV):
				//p . drawPie( int(x() - 30)+i , int(y() -30) , 60, 60 , 0 , 5760);
				p . drawArc( int(x() -30)+i, int(y()-30) , 60, 60, 0, 5760); 
				break; 
			default:
				break;
		}
	}
	InternetCar::draw( p );
}

/*
void DSRCCar:: dataRecv(Packet* pkt)
{
	InfoPacket *pp = (InfoPacket*) pkt;
	if(pp){
		for(InfoPacket* p = pos_list . first() ; p ; p = pos_list . next() ){
			if( p -> cID() == pp -> cID() ){
				pos_list . remove( p );
				break;
			}
		}
		pos_list . append( new InfoPacket(pp) );		
	}
}*/
void DSRCCar :: dataRecv(Packet* pkt)
{
	InfoPacket *pp = (InfoPacket*) pkt;

	InfoElement* p = pos_list . first ();
	while(p){
		if( p && (g_timer_ticks - p -> RecvTime() ) > 2*1000 ){
			pos_list . remove( p );
			p = pos_list . first ();
		}else
			p = pos_list . next();
	}
	if(pp){
		for(p = pos_list . first() ; p ; p = pos_list . next() ){
			if( pp -> cID() == p -> SndrID() ){
				pos_list . remove( p );
				break;
			}
		}
		pos_list . append( new InfoElement(pp,this) );		
	}
}

void DSRCCar :: drawTable( QListView* lv , QLabel* tl, QListBox* lb)
{
	bool left_not_save = false;
	bool right_not_save = false;
	TestLeftAndRight( left_not_save , right_not_save);

	lv -> clear();
	for(InfoElement* p = pos_list . first() ; p ; p = pos_list . next() ){
		new QListViewItem( lv , QString::number(p->SndrID()) 
			, QString::number(p->SndrX(),'f',2)
			, QString::number(p->SndrY(),'f',2)
			, QString::number(p->SndrSpeed(),'f',2)
			, QString::number(p->RecvTime()/1000.0,'f',2)
			, QString::number(p->RecvX(),'f',2)
			, QString::number(p->RecvY(),'f',2)
			, QString::number(p->RecvX() - p->SndrX(),'f',2)
			);
	}
	QString str;
	if( left_not_save ) str += "Left Not Save";
	else	str += "Left Save";
	str += "  ";
	if( right_not_save )	str += "Right Not Save";
	else	str += "Right Save";
	str += "\n";
	str += " (f) "+ Bulb2Str(frontBulb());
	str += " (b) "+ Bulb2Str(backBulb());
	if( change2RightLan() )
		str += " (c) -> toRight";
	if( change2LeftLan() )
		str += " (c) -> toLeft";

	for( int ch = 0 ; ch < CH_MAX ; ch ++ ){
		int num = ChDsrc2Number( ch );
		switch( ni1 . GetState(ch) ){
			case Phy::PHY_RECV:
				lb -> changeItem(QString::number(num)+" Recv" ,num);
				break;
			case Phy::PHY_IDLE:
				lb -> changeItem(QString::number(num)+" Idle" ,num);
				break;
			case Phy::PHY_SEND:
				lb -> changeItem(QString::number(num)+" Send" ,num);
				break;
			default:
				debug("@@");
				break; 
		}
	} 

	tl -> setText(str);
}

/*
void DSRCCar :: TestLeftAndRight( bool& lns, bool& rns)
{
	for(InfoElement * p = pos_list . first() ; p ; p = pos_list . next() ){
		if( DoubleAbs( p->SndrX() - p->RecvX() ) < ((gSafeDistance*10) * 2) ){ //四倍安全車距
			if( p->SndrY() != p->RecvY() && (DoubleAbs(  p->SndrY() - p->RecvY() ) <= 48.0) ){ //在一個車道之內
				if(p->RecvY() > p->SndrY() )
					rns = true;
				else if(p->RecvY() < p->SndrY() )
					lns = true;

			}
		}
	} 
}
void DSRCCar :: Try2ChangeLane()
{
	bool wanna_change_lane =false;
	if( change2LeftLan() || change2RightLan() || DriverBehavior() == BCTRL_MODE )
		return;
	switch( frontBulb() ){
		case BULB_RED:
			wanna_change_lane = true;
			break;
		case BULB_ORANGE:
			if( DriverBehavior() == AGGRESSIVE )
				wanna_change_lane = true;
			else if( DriverBehavior() == NORMAL)
				wanna_change_lane = true;
			break;
		default:
			break; 
	}
	if( g_timer_ticks - LastTimeChangeLane()  < 2000 )
		return;
	if(  DoubleAbs(realVelocity() - maxRealVelocity()) < 3.0 )
		return;

	if( realVelocity() < minRealVelocity() ) wanna_change_lane = true;
	if( wanna_change_lane == false && frontBulb() >= BULB_BLUE) return;

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
}


static inline double Square(double d)
{
	return d*d;
}

void DSRCCar :: UpdateWarnningLight(bool clear) 
{
	CAR_BULB save_front_bulb = BULB_NONE;
	CAR_BULB save_back_bulb = BULB_NONE;
	if( !clear ){
		save_front_bulb = frontBulb();
		save_back_bulb = backBulb();
	}

	double save_dis = (realVelocity() / 2)*10;
	for(InfoElement * p = pos_list . first() ; p ; p = pos_list . next() ){
		if( DoubleAbs( p -> SndrY() - p -> RecvY())
				> 1.5*CAR_HEIGHT) //在可視範圍外
			continue;
		BulbTestSet(save_front_bulb, BULB_GREEN);

		double DD = p->SndrX() - p->RecvX();
		double Dns = Square( KMHR_2_MS(p->SndrSpeed())*10.0 ) / (2*gFrictionFactorNormal*9.8);
		double Dnr = Square( KMHR_2_MS(realVelocity())*10.0 ) / (2*gFrictionFactorNormal*9.8)
			+ KMHR_2_MS(realVelocity())*10.0*(gHumanResponseTime+gSystemHandleTime)/1000.0;
		double Dnw =  DoubleAbs(Dns-Dnr)+10;
		double Des = Square( KMHR_2_MS(p->SndrSpeed())*10.0 ) / (2*gFrictionFactorEmerg*9.8);
		double Der = Square( KMHR_2_MS(realVelocity())*10.0 ) / (2*gFrictionFactorEmerg*9.8)
			+ KMHR_2_MS(realVelocity())*10.0*(gHumanResponseTime+gSystemHandleTime)/1000.0;
		double Dew =  DoubleAbs(Des-Der)+10;


		if(DD <= CAR_WIDTH){//前燈
			if(DoubleAbs(DD) < save_dis)
				BulbTestSet(save_front_bulb, BULB_BLUE);
			if(DoubleAbs(DD) < Dnw ){
			//	debug("(f) %d - DD = %f Dw = %f",nodeID(),DD,Dw);	
				BulbTestSet(save_front_bulb, BULB_ORANGE);
			}
			if(DoubleAbs(DD) < Dew ){
			//	debug("(f) %d - DD = %f Dw = %f",nodeID(),DD,Dw);	
				BulbTestSet(save_front_bulb, BULB_RED);
			}
		}
		if(DD >= -CAR_WIDTH ){//後燈
			if(DoubleAbs(DD) < save_dis)
				BulbTestSet(save_back_bulb, BULB_BLUE);
			if(DoubleAbs(DD) < Dnw ){
			//	debug("(f) %d - DD = %f Dw = %f",nodeID(),DD,Dw);	
				BulbTestSet(save_back_bulb, BULB_ORANGE);
			}
			if(DoubleAbs(DD) < Dew ){
			//	debug("(b) %d - DD = %f Dw = %f",nodeID(),DD,Dw);	
				BulbTestSet(save_back_bulb, BULB_RED);
			} 
		}

	} 
	setFrontBulb( save_front_bulb );
	setBackBulb( save_back_bulb );
	return;
}*/
