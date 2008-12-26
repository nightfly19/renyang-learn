

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

	qtimer.start( 20 );	// 每20毫秒執行一次

	ni_list.setAutoDelete( false );
	dsrc_ni_list.setAutoDelete( false );
}

NetCoordinator :: ~ NetCoordinator()
{
	ni_list.clear();
	dsrc_ni_list.clear();
}

void NetCoordinator :: Start()
{
	nc_state = NC_RUN;
	qtimer.start(20);

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

	//md.DumpDebug();

	//跟跟據Packet 產生對映的 Event, 用Packet的Length產生 接收Event arise的時間
	Event* p_event = new Event( pkt ,md, g_timer_ticks + 1);//先預設一秒後收到...
		
	//插入Event Q 並Sort
	event_q.InsertTail(p_event);
	return false;
}

void NetCoordinator :: event_active()
{
	if( nc_state != NC_RUN ){
		debug("net coordinator stop");
		qtimer.stop();	
	}
	if( event_q.NoEvent() )
		return;
	
	while( event_q.hasEvent() && g_timer_ticks > event_q.Head()->HdlTime() ){

//		debug("try to remove head szie = %u",event_q.Size() );
		Event* event = event_q.RemoveHead();	// 由queue中取出一個事件處理
		if( event->EventType() == Event::RECV_EVENT ){
			processRecvEvent(event);	// 執行接收事件
		}else if( event->EventType() == Event::SEND_EVENT){
			processSendEvent(event);	// 執行傳送事件
		}
		delete event;	// 傳送與接收完後,就把它刪掉
	}
}

void NetCoordinator :: processSendEvent( Event* )	// 傳送事件
{
	// 震驚!!?竟然沒有內容!!?也就是說沒有實作傳送事件?!!
}
void NetCoordinator :: processRecvEvent( Event* event)	// 接收事件
{
	// not implt yet
	InfoPacket* infoPacket = (InfoPacket*) event->PacketPtr();

	if( infoPacket ){
	//	debug("preocessEvent :: %e- (%e,%e)",event->HdlTime(),infoPacket->X(),infoPacket->Y());
	
		int sndrID = event->getMetaData().SndrID();
		if( event->getMetaData().PktType() == MetaData::PKT_802_11 ){
			for( NI_802_11* ni = ni_list.first(); ni ; ni = ni_list.next() ){	// 掃描所有的802.11的interface,目的是找出與傳送此封包的車子的其餘車子
				if( sndrID == ni->getCarNodeID() )	// 若傳送此封包的車子id與這一個interface車子的 id相同的話,則繼續下一個,設定成自已不能接收自己的封包
					continue;
				ni->BrRecv(infoPacket,event->getMetaData());
			}
		}else if( event->getMetaData().PktType() == MetaData::PKT_DSRC ){
			for( NI_DSRC* ni = dsrc_ni_list.first(); ni ; ni = dsrc_ni_list.next() ){
				if( sndrID == ni -> getCarNodeID() )
					continue;
				ni->BrRecv( infoPacket, event->getMetaData());
			}
		}
	}
}

void NetCoordinator :: addNI( NI_802_11* ni )
{
	ni_list.append ( ni );
}

void NetCoordinator :: addNI( NI_DSRC* ni )
{
	dsrc_ni_list.append ( ni );
}
bool NetCoordinator :: Send2Bellow(Packet*, MetaData) 
{	/*don't need*/ return false;	
};	
