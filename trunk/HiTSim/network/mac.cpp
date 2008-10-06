
#include "network/mac.h"
#include "network/module.h"
#include "network/packet.h"

Mac :: Mac ( QObject * parent, const char * name )
	: Module(parent,name)
{

}


/*
 * Virtual Function Implementation. 
 */
bool Mac :: Send2Bellow(Packet* pkt, MetaData md)
{
	if(chkBellow()){
		return bellow->RecvFromUpon(pkt, md);
	} 
	return false;
}
/*
 * Virtual Function Implementation. 
 */
bool Mac :: RecvFromUpon(Packet*, MetaData)
{
	return false;
}

bool Mac :: RecvFromBellow(Packet* pkt, MetaData)
{
	//debug("Mac :: RecvFromBellow");
/*	InfoPacket *pp = (InfoPacket*) pkt;
	if(pp){
	//	debug("Mac Receive From (%f,%f,%d)\n",pp->X(), pp->Y(), pp->cID() );
		emit sigPacketRecv(pp);
	}*/
	emit sigPacketRecv(pkt);
	return true;
}
bool Mac :: BrSend( Packet* pkt, MetaData md)
{
	md . setBrCast( true );
	return Send( pkt , md);
}

bool Mac :: Send( Packet* pkt, MetaData md)
{
//	debug("Mac %d try to send\n",ID());
	return Send2Bellow( pkt, md);
}
