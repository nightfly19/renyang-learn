
#ifndef __DSRC_NETWORK_INTERFACE_H__
#define __DSRC_NETWORK_INTERFACE_H__

#include <qobject.h>
#include "network/ni.h"
#include "network/mac.h"
#include "network/mac_dsrc.h"
#include "network/phy.h"
#include "network/phy_dsrc.h"
#include "pos.h"
#include "network/MetaData.h"
#include "network/dsrc_global.h"

class Packet;
class NetCoordinator;
class DSRCCar;

class NI_DSRC // Network Interface 
	: public NI 
{
Q_OBJECT
public:
	NI_DSRC ( QObject * parent = 0, const char * name = 0 );

	void setNetCoor(NetCoordinator*);

	bool BrSend(Packet* pkt, int channel );
	bool BrSend(Packet* pkt){	return BrSend(pkt,CH_NORMAL1);	};
	bool BrRecv(Packet* pkt, MetaData md);

	int ID();

	int GetState(int ch);
	bool isBusy(int ch);
	bool isIdle(int ch);

	bool isSending(){	return phy.isSending();	};
	bool isRecving(){	return phy.isRecving();	};

	int getCarNodeID();

public slots:
	void dataRecv(Packet*); //arise from mac layer
signals:
	void sigPacketRecv(Packet*);


protected:
	MacDSRC mac;
	PhyDSRC phy;
	DSRCCar* car;

};

#endif 

