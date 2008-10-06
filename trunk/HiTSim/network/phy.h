

#ifndef __PHY_H__
#define __PHY_H__

#include <qobject.h>
#include "pos.h"
#include "network/module.h"
#include "network/MetaData.h"

class Packet;
class Phy // Network Interface 
	: public Module
{
Q_OBJECT
public:
	enum PHY_STATE{
		PHY_SEND,
		PHY_RECV,
		PHY_IDLE
	};

	Phy ( QObject * parent = 0, const char * name = 0 );
	
	/*
	virtual int GetState() = 0;
	virtual bool isBusy() = 0;
	virtual bool isIdle() = 0;
	*/

//	bool Send(Packet* pkt, int channel, bool brcst);

	virtual bool Send2Bellow(Packet*, MetaData md =MetaData()) = 0;
	virtual bool RecvFromUpon(Packet* pkt, MetaData md =MetaData()) = 0;
	virtual bool RecvFromBellow( Packet* pkt, MetaData) = 0;

private:
	PHY_STATE _phy_state;
	double _busy_until;
};

#endif

