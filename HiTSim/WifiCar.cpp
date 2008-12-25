
#include <qpainter.h>
#include <qlistview.h>
#include <qlistbox.h>
#include <qlabel.h>
#include "network/packet.h"
#include "Car.h"
#include "WifiCar.h"
#include "InfoElt.h"
#include "global.h"

WifiCar::WifiCar( QCanvas* canvas)
	//: Car( canvas)
	: InternetCar( canvas)
	, ni1( this )
{
	connect( &ni1, SIGNAL( sigPacketRecv(Packet*) ) , this , SLOT(dataRecv(Packet*)));
}

bool WifiCar::BrLocation() // broadcast location
{
	if( ni1 . isBusy() )	return false; // 先測試是否idle
	InfoPacket *p = new InfoPacket(  realX(), realY() , nodeID() , realVelocity() );

	return ni1.BrSend( p );
}

void WifiCar:: setNetCoor(NetCoordinator* nc)
{
	ni1 . setNetCoor(nc);

	nc -> addNI( &ni1 );
}
void WifiCar::draw( QPainter & p )
{
	if( ni1 . GetState() == NI :: NI_SEND){
		p . setPen( blue );
		p . setBrush( blue );
		p . drawArc( int(x() -1000) , int(y()-1000) , 2000, 2000 , 5760 , 5760);
		//p . drawArc( int(x() -50) , int(y()-50) , 100, 100 , 5760 , 5760);
		p . drawPie( int(x() - 50)  , int(y() -50) , 100, 100 , 0 , 5760);
	}else if( ni1 . GetState() == NI:: NI_RECV){
		p . setPen( red );
		p . setBrush( red );
		//p . drawPie( int(x() - 30)  , int(y() -30) , 60, 60 , 0 , 5760);
		p . drawArc( int(x() - 30)  , int(y() -30) , 60, 60 , 0 , 5760);
	}
	InternetCar::draw( p );
}

void WifiCar:: dataRecv(Packet* pkt)
{
	InfoPacket *pp = (InfoPacket*) pkt;	// 接收到的封包(InfoPacket)

	InfoElement* p = pos_list.first ();	// pos_list是記錄本車收到的封包
	while(p){	// 把所有列表上超過livetime的封包刪除
		if( p && (g_timer_ticks - p->RecvTime() ) > 2*1000 ){ // 當時間超過2000則表示超過livetime,則要移除此封包
			pos_list.remove( p );	// 移除此封包(也是第一個封包)
			p = pos_list.first ();	// 移動到第一個封包
		}else
			p = pos_list.next();
	}
	if(pp){	// 準備把接收到的封包加入本車收到的封包列中
		for(p = pos_list.first() ; p ; p = pos_list.next() ){
			if( pp->cID() == p->SndrID() ){	// 當封包傳送車的id與封包接收者的id相同時,刪
				pos_list.remove( p );
				break;
			}
		}
		pos_list.append( new InfoElement(pp,this) );	// 加入封包到此interface的封包列中
	}
}
void WifiCar:: drawTable( QListView* lv , QLabel* tl, QListBox* lb)	// 顯示本車收到的封包列
{
	bool left_not_save = false;
	bool right_not_save = false;
	TestLeftAndRight( left_not_save , right_not_save);

	lv -> clear();
	for(InfoElement* p = pos_list . first() ; p ; p = pos_list.next() ){
		new QListViewItem( lv , QString::number(p->SndrID()) 
			, QString::number(p->SndrX(),'f',2)
			, QString::number(p->SndrY(),'f',2)
			, QString::number(p->SndrSpeed(),'f',2)
			, QString::number(p->RecvTime()/1000.0,'f',2)
			, QString::number(p->RecvX(),'f',2)
			, QString::number(p->RecvY(),'f',2)
			, QString::number(p->RecvX() - p->SndrX(),'f',2)
			);
	}
	QString str;
	if( left_not_save ) str += "Left Not Save";
	else	str += "Left Save";
	str += "  ";
	if( right_not_save )	str += "Right Not Save";
	else	str += "Right Save";
	str += "\n";
	str += " (f) "+ Bulb2Str(frontBulb());
	str += " (b) "+ Bulb2Str(backBulb());
	if( change2RightLan() )
		str += " (c) -> toRight";
	if( change2LeftLan() )
		str += " (c) -> toLeft";

	int num = ni1.Channel();
	switch( ni1 . GetState() ){
		case Phy::PHY_RECV:
			lb -> changeItem(QString::number(num)+" Recv" ,num);
			break;
		case Phy::PHY_IDLE:
			lb -> changeItem(QString::number(num)+" Idle" ,num);
			break;
		case Phy::PHY_SEND:
			lb -> changeItem(QString::number(num)+" Send" ,num);
			break;
		default:
			break; 
	}

	tl -> setText(str);
}

