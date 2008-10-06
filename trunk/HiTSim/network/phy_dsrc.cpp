
#include <math.h>

#include "network/phy_dsrc.h"
#include "network/packet.h" 
#include "global.h"

PhyDSRC:: PhyDSRC( QObject * parent, const char * name )
	: Phy(parent,name)
{

	for(int i = 0 ; i < CH_MAX ; i ++){
		_phy_state[i] = PHY_IDLE;
		_busy_until[i] = 0;
	}
}

/*
bool PhyDSRC :: Send(Packet* pkt, int channel, bool brcst)
{
	return false;
}
*/

bool PhyDSRC :: Send2Bellow(Packet* pkt,MetaData md)
{
	if(chkBellow()){
		_phy_state[ md . Channel() ] = PHY_SEND;
		_busy_until[ md . Channel() ] = g_timer_ticks + 10; //TODO: 要寫上正確的數據 10/1000
		//送到Network Coordinator
		return bellow->RecvFromUpon(pkt, md);
	} 
	return false;
}
bool PhyDSRC :: RecvFromUpon(Packet* pkt, MetaData md)
{
//	debug("PhyDSRC %d recv from upon\n",ID());
	Send2Bellow(pkt,md);
	return false;
}

/*
static inline double distance( Pos p1, Pos p2 )
{
	return sqrt( (p1.X()-p2.X()) * (p1.X()-p2.X())
		+ (p1.Y()-p2.Y()) * (p1.Y()-p2.Y() ) );
}*/
bool PhyDSRC :: RecvFromBellow( Packet* pkt, MetaData md )
{
	double dis = md.Distance();
	
	if(dis<2000){
		if(dis < 1000){
//			return (upon)? upon->RecvFromBellow(pkt,md) :false;
	//		debug("dsrc phy recv succes");
			if( isBusy(md.Channel()) ){
				g_net_collision ++;
				return false;
			}
			if(upon){
				_phy_state[ md . Channel() ] = PHY_RECV;
				_busy_until[ md . Channel() ] = g_timer_ticks + 10; //TODO: 要寫上正確的數據
				return upon->RecvFromBellow(pkt,md);
			}else
				return false;
		}
	}else{
		return false;
	}

	return false;
}

int PhyDSRC :: GetState(int ch)
{
	//if( DSRC
	if( _phy_state[ch] != PHY_IDLE ){
		if( g_timer_ticks > _busy_until[ ch ] )
			_phy_state[ch] = PHY_IDLE;
	}
	
	return _phy_state[ch];
}

bool PhyDSRC :: isBusy(int ch)
{
	/*
	if(ch == -1){
		return false;
	}*/
	if( GetState(ch) == PHY_IDLE )
		return false;
	else
		return true;
}

bool PhyDSRC :: isIdle(int ch)
{
	return !isBusy(ch);
}

bool PhyDSRC :: isSending()
{
	for(int i = 0 ; i < CH_MAX ; i ++){
		if(_phy_state[i] == PHY_SEND )
			return true; 
	}
	return false;

}
bool PhyDSRC :: isRecving()
{
	for(int i = 0 ; i < CH_MAX ; i ++){
		if(_phy_state[i] == PHY_RECV)
			return true; 
	}
	return false;
}
