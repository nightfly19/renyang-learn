

#ifndef __DSRC_PHY_H__
#define __DSRC_PHY_H__

#include <qobject.h>
#include "pos.h"
#include "network/module.h"
#include "network/phy.h"
#include "network/packet.h"
#include "network/MetaData.h"
#include "network/dsrc_global.h"

//class Packet;
class PhyDSRC // Network Interface 
	//: public Module
	: public Phy 
{
Q_OBJECT
public:

	PhyDSRC ( QObject * parent = 0, const char * name = 0 );
	
	int GetState(int ch);
	bool isBusy(int ch);
	bool isIdle(int ch);
	bool isSending(); //test all channel
	bool isRecving(); //test all channel

//	bool Send(Packet* pkt, int channel, bool brcst);

	virtual bool Send2Bellow(Packet*, MetaData md =MetaData());
	virtual bool RecvFromUpon(Packet* pkt, MetaData md =MetaData());

	virtual bool RecvFromBellow( Packet* pkt, MetaData);

private:
	PHY_STATE _phy_state[CH_MAX];
	double _busy_until[CH_MAX];
};

#endif

