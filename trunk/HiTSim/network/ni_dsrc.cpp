
#include "network/ni_dsrc.h"
#include "network/packet.h"
#include "network/net_coor.h"
#include "WifiCar.h"
#include "DSRCCar.h"
#include "global.h"

NI_DSRC :: NI_DSRC ( QObject * parent, const char * name )
	: NI(parent,name)
	, mac(this,name)
	, phy(this,name)
	, car((DSRCCar*) parent)
{
	mac . setBellow( &phy );
	phy . setUpon( &mac );

	connect( &mac , SIGNAL( sigPacketRecv(Packet*) ) , this , SLOT(dataRecv(Packet*))); 
}
bool NI_DSRC :: BrSend( Packet* pkt, int channel)
{
	MetaData md( Pos(car->realX(),car->realY()) , Pos(0,0), channel, true, MetaData::PKT_DSRC);
	md . setSndrID( car-> nodeID() );

	mac . BrSend( pkt , md);

	return true;
}

/*
 * From NetCoordinator, and add some meta data to Phy
 */
bool NI_DSRC :: BrRecv( Packet* pkt, MetaData md)
{
	if(car == NULL)	return false;
	if( md . SndrID() == car -> nodeID() )
		return false;

	Pos recvPos( car->realX(), car->realY() );
	md . setRecvPos( Pos(car->realX(), car->realY() ));

	/*
	debug("ni BrRecv");
	md . DumpDebug();
	*/

	if( phy . RecvFromBellow( pkt , md)){
		return true;
	}else
		return false;
}

void NI_DSRC :: setNetCoor( NetCoordinator* nc )
{
	phy . setBellow( nc );

	nc -> addNI( this );
}

int NI_DSRC :: GetState(int ch)
{
	if( phy . GetState(ch) == Phy::PHY_IDLE )
		_ni_state = NI_IDLE; 
	else if( phy . GetState(ch) == Phy::PHY_RECV)
		_ni_state = NI_RECV; 
	else if( phy . GetState(ch) == Phy::PHY_SEND)
		_ni_state = NI_SEND; 
	
	return _ni_state;
}

bool NI_DSRC :: isBusy(int ch)
{
	if( GetState(ch) == NI_IDLE )
		return false;
	else
		return true;
}

bool NI_DSRC :: isIdle(int ch)
{
	if( GetState(ch) == NI_IDLE )
		return true;
	else
		return false; 
}
int NI_DSRC :: getCarNodeID()
{	
	return (car)? (car->nodeID()) : 0;	
};

void NI_DSRC :: dataRecv(Packet* pkt)
{
	InfoPacket *pp = (InfoPacket*) pkt;
	if(pp)
		emit sigPacketRecv(pkt);
}

int NI_DSRC :: ID()
{
	if(car)
		return car->nodeID();

	return 0;
}
