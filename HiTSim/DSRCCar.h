
#ifndef __DSRC_CAR_H__
#define __DSRC_CAR_H__

#include "Car.h"
#include "InternetCar.h"
#include "network/ni_dsrc.h"
#include "network/net_coor.h"
#include <qptrlist.h>

class NetCoordinator;
class Packet;
class InfoPacket;
class InfoElement;
class QListView;
class QListBox;
class QLabel;

//class DSRCCar : public Car
class DSRCCar : public InternetCar
{
Q_OBJECT
	public:
		enum CHCH_MODE{	//Channel Chosen Mode
			CHCH_POLL,
			CHCH_ID_POLL
		};
	public: 
		DSRCCar(QCanvas*);
		bool BrLocation();
	

		void setNetCoor(NetCoordinator* );
		void drawTable( QListView*, QLabel*, QListBox*);

		virtual CAR_TYPE CarType(){	return CT_DSRC;	};
	private slots:
		void dataRecv(Packet* pkt);
	protected:
		virtual void draw( QPainter & p );

	private:
		//NI ni1;
		NI_DSRC ni1;

	public:
		void setChChMode(CHCH_MODE m){	chchmode = m;	};
	private:
		int ChannelPolling();
		int ChannelWarnPolling();
		int ChannelPollingMod4();
		int ChannelWarnPollingMod2();
		CHCH_MODE ChChMode(){	return chchmode;	};	//Channel Chosen Mode
		CHCH_MODE chchmode;

};
#endif

