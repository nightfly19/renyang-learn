
#ifndef __INTERNET_CAR_H__
#define __INTERNET_CAR_H__


#include "Car.h"
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

class InternetCar : public Car
{
Q_OBJECT
	public: 
		InternetCar(QCanvas*);
		virtual void setNetCoor(NetCoordinator* ) = 0;

		virtual void drawTable( QListView*, QLabel* ,QListBox*) = 0;
		virtual void calNextVelocity(double);

		bool SmartBrake(){	return smart_brake;	};
		void setSmartBrake(bool b){	smart_brake = b;	};
	private slots:
		virtual void dataRecv(Packet* pkt) = 0;

	protected:
		virtual void draw( QPainter & p );

	/* 自動駕駛 */
	public:
		virtual void UpdateWarnningLight(bool clear = false); /* imp in Wifi Car */
		virtual void UpdateCarByWarnning();
		virtual void Try2ChangeLane();
	protected:
		virtual void TestLeftAndRight( bool& lns, bool& rns);
	/* 自動駕駛 */

	private:
		double front_closest_speed;
		bool smart_brake;
	protected:
		QPtrList<InfoElement> pos_list; // 建立一個double-link用來記錄收到的封包資訊
};

#endif
