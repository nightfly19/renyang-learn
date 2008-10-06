
#include <math.h>

#include "network/phy_802_11.h"
#include "network/packet.h" 
#include "global.h"

Phy802_11:: Phy802_11( QObject * parent, const char * name )
	: Phy(parent,name)
	, _phy_state( PHY_IDLE )
	, _busy_until(0)
	, _channel(1)
{

}

/*
bool Phy802_11 :: Send(Packet* pkt, int channel, bool brcst)
{
	return false;
}
*/

bool Phy802_11 :: Send2Bellow(Packet* pkt,MetaData md)
{
	if(chkBellow()){
		md . setChannel( Channel() );
		_phy_state = PHY_SEND;
		_busy_until = g_timer_ticks + 10; //TODO: 要寫上正確的數據 10/1000
		//送到Network Coordinator
		return bellow->RecvFromUpon(pkt, md);
	} 
	return false;
}
bool Phy802_11 :: RecvFromUpon(Packet* pkt, MetaData md)
{
//	debug("Phy802_11 %d recv from upon\n",ID());
	Send2Bellow(pkt,md);
	return false;
}

bool Phy802_11 :: RecvFromBellow( Packet* pkt, MetaData md )
{
	double dis = md.Distance();

	if( md . Channel() != Channel() )
		return false;
	
	if(dis<2000){
		if(dis < 1000){
//			return (upon)? upon->RecvFromBellow(pkt,md) :false;
			if( isBusy() ){
				g_net_collision ++;
				return false;
			}
			if(upon){
				_phy_state = PHY_RECV;
				_busy_until = g_timer_ticks + 10; //TODO: 要寫上正確的數據 10/1000
				return upon->RecvFromBellow(pkt,md);
			}else
				return false;
		}
	}else{
		return false;
	}

	return false;
}

int Phy802_11 :: GetState()
{
	if( _phy_state != PHY_IDLE ){
		if( g_timer_ticks > _busy_until )
			_phy_state = PHY_IDLE;
	}
	
	return _phy_state;
}

bool Phy802_11 :: isBusy()
{
	if( GetState() == PHY_IDLE )
		return false;
	else
		return true;
}

bool Phy802_11 :: isIdle()
{
	if( GetState() == PHY_IDLE )
		return true;
	else
		return false; 
}
