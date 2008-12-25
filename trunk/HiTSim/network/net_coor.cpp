

#include "network/net_coor.h"
#include "network/packet.h"
#include "event/event.h"
#include "global.h"


NetCoordinator :: NetCoordinator(QObject* parent , const char* name )
	: Module(parent , name )
	, nc_state( NC_STOP )
	, qtimer( this, "Timer for NetCoor")
	, event_q()
	, ni_list()
{
	connect( &qtimer, SIGNAL( timeout() ), this, SLOT( event_active() ) );

	qtimer . start( 20 );

	ni_list . setAutoDelete( false );
	dsrc_ni_list . setAutoDelete( false );
}

NetCoordinator :: ~ NetCoordinator()
{
	ni_list . clear();
	dsrc_ni_list . clear();
}

void NetCoordinator :: Start()
{
	nc_state = NC_RUN;
	qtimer . start( 20 );

}

void NetCoordinator :: Stop()
{
	nc_state = NC_STOP;
}

void NetCoordinator :: Pause()
{
	nc_state = NC_PAUSE;
}

bool NetCoordinator :: RecvFromUpon( Packet* pkt, MetaData md)
{
	/* Debug Msg
	 */
	//InfoPacket* pos = (InfoPacket*)(pkt);

	//md . DumpDebug();

	//跟跟據Packet 產生對映的 Event, 用Packet的Length產生 接收Event arise的時間
	Event* p_event = new Event( pkt ,md, g_timer_ticks + 1);//先預設一秒後收到...
		
	//插入Event Q 並Sort
	event_q . InsertTail( p_event );
	return false;
}

void NetCoordinator :: event_active()
{
	if( nc_state != NC_RUN ){
		debug("net coordinator stop");
		qtimer . stop();	
	}
	if( event_q . NoEvent() )
		return;
	
	while( event_q . hasEvent() 
		&& g_timer_ticks > event_q . Head() -> HdlTime() ){

//		debug("try to remove head szie = %u",event_q . Size() );
		Event* event = event_q . RemoveHead();
		if( event -> EventType() == Event::RECV_EVENT ){
			processRecvEvent( event ); 
		}else if( event -> EventType() == Event::SEND_EVENT){
			processSendEvent( event ); 
		}
		delete event;
	}
}

void NetCoordinator :: processSendEvent( Event* )
{

}
void NetCoordinator :: processRecvEvent( Event* event)	// 接收事件
{
	// not implt yet
	InfoPacket* infoPacket = (InfoPacket*) event->PacketPtr();

	if( infoPacket ){
	//	debug("preocessEvent :: %e- (%e,%e)",event->HdlTime(),infoPacket->X(),infoPacket->Y());
	
		int sndrID = event->getMetaData() . SndrID();
		if( event->getMetaData() .  PktType() == MetaData::PKT_802_11 ){
			for( NI_802_11* ni = ni_list . first(); ni ; ni = ni_list . next() ){
				if( sndrID == ni -> getCarNodeID() )
					continue;
				ni -> BrRecv( infoPacket, event->getMetaData());
			}
		}else if( event->getMetaData() . PktType() == MetaData::PKT_DSRC ){
			for( NI_DSRC* ni = dsrc_ni_list . first(); ni ; ni = dsrc_ni_list . next() ){
				if( sndrID == ni -> getCarNodeID() )
					continue;
				ni -> BrRecv( infoPacket, event->getMetaData());
			}
		}
	}
}

void NetCoordinator :: addNI( NI_802_11* ni )
{
	ni_list . append ( ni );
}

void NetCoordinator :: addNI( NI_DSRC* ni )
{
	dsrc_ni_list . append ( ni );
}
bool NetCoordinator :: Send2Bellow(Packet*, MetaData) 
{	/*don't need*/ return false;	
};	
