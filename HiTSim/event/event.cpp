
#include "qobject.h"
#include "event.h"
#include "network/packet.h"

Event :: Event()
	: EventElt()
	, hdl_time(0)
	, md()
	, _event_type( RECV_EVENT)	// 預設是接收事件
{
}

Event :: Event(Packet* p , MetaData m, double time, EVT_TYPE et )
	: EventElt()
	, p_pkt( p )
	, hdl_time(time)
	, md(m)
	, _event_type( et )
{
}

Event :: ~Event()
{
//	~EventElt();
	delete p_pkt;
	p_pkt = NULL;
}
