

#ifndef __EVENT_H__
#define __EVENT_H__

#include "event/event_elt.h"
#include "network/MetaData.h"

class Packet;
class Event : public EventElt
{
public:
	enum EVT_TYPE{
		SEND_EVENT,
		RECV_EVENT 
	};
public:
	Event();
	Event(Packet*,MetaData md,double,EVT_TYPE et = RECV_EVENT);
	~Event();
	
	MetaData& getMetaData(){	return md;	};
	void setMetaData( MetaData m ){	md = m;	};

	double HdlTime(){	return hdl_time;	};
	void setHdlTime( double time ){	hdl_time = time;	};

	Packet* PacketPtr(){	return p_pkt;	};
	void setPacketPrt( Packet* p ){	p_pkt = p;	};

	EVT_TYPE EventType(){	return _event_type;	};
	void setEventType(EVT_TYPE et){	_event_type = et;	};
private:
	Packet* p_pkt;
	double hdl_time;
	MetaData md;
	EVT_TYPE _event_type;
}; 

#endif
