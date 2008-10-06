
#include <qobject.h>
#include <math.h>
#include "network/MetaData.h"


MetaData :: MetaData( Pos s , Pos r , int channel , bool br, PKT_TYPE pkt_type)
	: _channel(channel)
	, _brcast(br)
	, _sndrPos(s)
	, _sndrID(0)
	, _recvPos(r)
	, _recvID(0)
	, _pkt_type(pkt_type)
{

}

void MetaData :: DumpDebug()
{
	debug("SndrPos (%e,%e) RecvPos (%e,%e) Channel %d"
		,_sndrPos.X(),_sndrPos.Y()
		,_recvPos.X(),_recvPos.Y()
		,_channel);
}

double MetaData :: Distance( )
{
	return sqrt( (_sndrPos.X()-_recvPos.X()) * (_sndrPos.X()-_recvPos.X())
		+ (_sndrPos.Y()-_recvPos.Y()) * (_sndrPos.Y()-_recvPos.Y() ) );
}
