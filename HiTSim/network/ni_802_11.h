
#ifndef __802_11_NETWORK_INTERFACE_H__
#define __802_11_NETWORK_INTERFACE_H__

#include <qobject.h>
#include "network/mac.h"
#include "network/phy.h"
#include "network/phy_802_11.h"
#include "pos.h"
#include "network/MetaData.h"
#include "network/ni.h"

class Mac;
class Phy;
class Packet;
class NetCoordinator;
class WifiCar;

class NI_802_11 // Network Interface 
	: public NI 
{
Q_OBJECT
public:
	NI_802_11 ( QObject * parent = 0, const char * name = 0 );
	void setNetCoor(NetCoordinator*);

	bool BrSend(Packet* pkt );
	bool BrRecv(Packet* pkt, MetaData md);

	int ID();
	int Channel(){	return phy.Channel();	};

	virtual int GetState();
	virtual bool isBusy();
	virtual bool isIdle();

	int getCarNodeID();

public slots:
	void dataRecv(Packet*); //arise from mac layer
signals:
	void sigPacketRecv(Packet*);

protected:
	Mac mac;
	Phy802_11 phy;
	WifiCar* car;

};

#endif 

