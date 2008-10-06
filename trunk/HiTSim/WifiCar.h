
#ifndef __WIFI_CAR_H__
#define __WIFI_CAR_H__

#include "Car.h"
#include "InternetCar.h"
#include "network/ni_802_11.h"
#include "network/net_coor.h"
#include "InfoElt.h"
#include <qptrlist.h>

class NetCoordinator;
class Packet;
class InfoPacket;
class InfoElement;
class QListView;
class QListBox;
class QLabel;

//class WifiCar : public Car
class WifiCar : public InternetCar
{
Q_OBJECT
	public: 
		WifiCar(QCanvas*);
		bool BrLocation();
	

		void setNetCoor(NetCoordinator* );
		void drawTable( QListView*, QLabel* ,QListBox*);

		virtual CAR_TYPE CarType(){	return CT_WIFI;	};
	private slots:
		void dataRecv(Packet* pkt);
	protected:
		virtual void draw( QPainter & p );
	
	private:
		NI_802_11 ni1;
	/*
	public:
		QPtrList<InfoElement> pos_list;
		*/

};
#endif
