

#ifndef __802_11_PHY_H__
#define __802_11_PHY_H__

#include <qobject.h>
#include "pos.h"
#include "network/module.h"
#include "network/phy.h"
#include "network/MetaData.h"

class Packet;
class Phy802_11 // Network Interface 
	: public Phy 
{
Q_OBJECT
public:

	Phy802_11 ( QObject * parent = 0, const char * name = 0 );
	
	virtual int GetState();
	virtual bool isBusy();
	virtual bool isIdle();

//	bool Send(Packet* pkt, int channel, bool brcst);

	virtual bool Send2Bellow(Packet*, MetaData md =MetaData());
	virtual bool RecvFromUpon(Packet* pkt, MetaData md =MetaData());
	virtual bool RecvFromBellow( Packet* pkt, MetaData);

	int Channel(){	return _channel;	};
	void setChannel(int c){	_channel = c;	};

private:
	PHY_STATE _phy_state;
	double _busy_until;

	int _channel;
};

#endif

