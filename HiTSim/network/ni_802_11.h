
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

class NI_802_11 // Network Interface 建立802.11的interface
	: public NI 
{
Q_OBJECT
public:
	NI_802_11 ( QObject * parent = 0, const char * name = 0 );
	void setNetCoor(NetCoordinator*); // 設定由哪一個coordinator控制此interface

	bool BrSend(Packet* pkt ); // 傳送封包
	bool BrRecv(Packet* pkt, MetaData md); // 接收封包

	int ID();	// 傳回擁有這一個interface的車子id
	int Channel(){	return phy.Channel();	};	// 傳回這一個interface使用的channel

	virtual int GetState();	// 傳回目前這一個interface的狀態
	virtual bool isBusy();	// 目前此interface是否忙錄
	virtual bool isIdle();	// 目前此interface是否閒置

	int getCarNodeID();	// 與ID()是做一樣的事情

public slots:
	void dataRecv(Packet*); // 由使用者可以操作的介面
signals:
	void sigPacketRecv(Packet*); // 不懂

protected:
	Mac mac;	// 記錄目前這一個interface是用哪一個mac
	Phy802_11 phy;	// 記錄目前這一個interface是用哪一個phy
	WifiCar* car;	// 記錄目前這一個interface是哪一台車子的

};

#endif 

