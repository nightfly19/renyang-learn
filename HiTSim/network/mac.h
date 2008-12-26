
#ifndef __MAC_H__
#define __MAC_H__

#include <qobject.h>
#include "network/module.h"

class Packet;
class Mac // Network Interface 
	: public Module
{
Q_OBJECT
public:
	Mac ( QObject * parent = 0, const char * name = 0 );
	

public:
	virtual bool Send(Packet*, MetaData md = MetaData() );
	virtual bool BrSend(Packet*, MetaData md = MetaData() ); // for Network Interface

	virtual bool Send2Bellow(Packet*, MetaData md = MetaData()); // for Phy
	virtual bool RecvFromUpon(Packet* pkt, MetaData md = MetaData()); // from ?
	virtual bool RecvFromBellow(Packet* pkt, MetaData md);	// 由下層接收資料,是由phy傳送過來

signals:
	void sigPacketRecv(Packet*);

};


#endif

