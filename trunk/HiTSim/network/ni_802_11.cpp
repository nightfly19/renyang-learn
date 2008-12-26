
#include "network/packet.h"
#include "network/ni_802_11.h"
#include "network/net_coor.h"
#include "WifiCar.h"
#include "global.h"

NI_802_11 :: NI_802_11 ( QObject * parent, const char * name )
	: NI (parent,name)
	, mac(this,name)
	, phy(this,name)
	, car((WifiCar*) parent)
{
	mac.setBellow( &phy );	// 設定mac的下層是phy
	phy.setUpon( &mac );	// 設定phy的上層是mac

	connect( &mac , SIGNAL( sigPacketRecv(Packet*) ) , this , SLOT(dataRecv(Packet*)));
}


bool NI_802_11 :: BrSend( Packet* pkt)
{
	MetaData md( Pos(car->realX(),car->realY()) , Pos(0,0), Channel() , true, MetaData::PKT_802_11);
	md.setSndrID( car-> nodeID() );
	mac.BrSend( pkt , md);

//	_busy_until = g_timer_ticks + 10; //TODO:查出確定的數字		
//	_ni_state = NI_SEND;

	return true;
}

/*
 * From NetCoordinator, and add some meta data to Phy
 */
bool NI_802_11 :: BrRecv( Packet* pkt, MetaData md)	// 接收封包
{
	if(car == NULL)	return false;
	if( md.SndrID() == car->nodeID() )	// 當傳送此封包車子的id與此interface的車子id是相同的話,回傳false
		return false;

	Pos recvPos( car->realX(), car->realY() );	// 取出接收到時車子的位置
	md.setRecvPos( Pos(car->realX(), car->realY() ));	// 設定接收到的位置

	/*
	debug("ni BrRecv");
	md.DumpDebug();
	*/

	if( phy.RecvFromBellow( pkt , md)){	// 由下層接收到,應該就是net coordinator
//		_busy_until = g_timer_ticks + 10; //TODO	10/1000 !?
//		_ni_state = NI_RECV;

		return true;
	}else
		return false;
}

/*
void NI :: setID( int i)
{
	id = i;
	mac.setID( id );
	phy.setID( id );
}*/

void NI_802_11 :: setNetCoor( NetCoordinator* nc )
{
//	NetCoordinator n;
	phy.setBellow( nc );

	nc -> addNI( this );
}

int NI_802_11 :: getCarNodeID()
{	
	return (car)? (car->nodeID()) : 0;	
};

void NI_802_11 :: dataRecv(Packet* pkt)	
{
	InfoPacket *pp = (InfoPacket*) pkt;
	if(pp)
		emit sigPacketRecv(pkt);	// 觸發的是this->sigPacketRecv(pkt);
	// 好像connect( &mac , SIGNAL( sigPacketRecv(Packet*) ) , this , SLOT(dataRecv(Pa    cket*)));與此函數會無限迴圈哩
}

int NI_802_11 :: ID()
{
	if(car)
		return car->nodeID();
	return 0;
}
int NI_802_11 :: GetState()
{
	/* 舊的 程式碼...現在用Phy的State就行了...
	if( _ni_state != NI_IDLE ){
		if( g_timer_ticks > _busy_until )
			_ni_state = NI_IDLE;

	}*/
	if( phy.GetState() == Phy::PHY_IDLE )
		_ni_state = NI_IDLE; 
	else if( phy.GetState() == Phy::PHY_RECV)
		_ni_state = NI_RECV; 
	else if( phy.GetState() == Phy::PHY_SEND)
		_ni_state = NI_SEND; 
	
	return _ni_state;
}

bool NI_802_11 :: isBusy()
{
	if( GetState() == NI_IDLE )
		return false;
	else
		return true;
}

bool NI_802_11 :: isIdle()
{
	if( GetState() == NI_IDLE )
		return true;
	else
		return false; 
}
