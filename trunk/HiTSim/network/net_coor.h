
#ifndef __NET_COOR_H__
#define __NET_COOR_H__

#include <qobject.h>
#include <qtimer.h>
#include <qptrlist.h>

#include "network/module.h"
#include "event/event_q.h"
#include "network/ni_802_11.h"
#include "network/ni_dsrc.h"

class Event;
class NetCoordinator : public Module
{
	Q_OBJECT

public:
	enum NC_STATE{
		NC_RUN,
		NC_STOP,
		NC_PAUSE
	};

public:	
	NetCoordinator(QObject* parent = 0 , const char* name = 0);
	~NetCoordinator();
	void processRecvEvent(Event*);
	void processSendEvent(Event*);

	void Start();
	void Pause();
	void Stop();

public slots:
	void event_active();
	void slotSimDone(){	Pause();	};

public: // From Module 
	virtual bool Send2Bellow(Packet*, MetaData md = MetaData());	
	virtual bool RecvFromUpon(Packet* pkt, MetaData md = MetaData());

	void addNI( NI_802_11* );
	void addNI( NI_DSRC* );

private:
	NC_STATE nc_state;
	QTimer qtimer;
	EventQ	event_q;

	QPtrList<NI_802_11> ni_list;
	QPtrList<NI_DSRC> dsrc_ni_list;
};

#endif
