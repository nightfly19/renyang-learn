
#include <math.h>

#include "network/phy_802_11.h"
#include "network/packet.h" 
#include "global.h"

Phy802_11:: Phy802_11( QObject * parent, const char * name )
	: Phy(parent,name)
	, _phy_state( PHY_IDLE )
	, _busy_until(0)
	, _channel(1)
{

}

/*
bool Phy802_11 :: Send(Packet* pkt, int channel, bool brcst)
{
	return false;
}
*/

bool Phy802_11 :: Send2Bellow(Packet* pkt,MetaData md)
{
	if(chkBellow()){
		md.setChannel( Channel() );
		_phy_state = PHY_SEND;
		_busy_until = g_timer_ticks + 10; //TODO: 要寫上正確的數據 10/1000;傳送延遲的時間
		//送到Network Coordinator
		return bellow->RecvFromUpon(pkt, md);
	} 
	return false;
}
bool Phy802_11 :: RecvFromUpon(Packet* pkt, MetaData md)
{
//	debug("Phy802_11 %d recv from upon\n",ID());
	Send2Bellow(pkt,md);
	return false;
}

bool Phy802_11 :: RecvFromBellow( Packet* pkt, MetaData md )	// 由net coordinator的BrRecv內呼叫此程式
{
	double dis = md.Distance();	// 計算此封包由傳送到接收的距離差

	if( md.Channel() != Channel() )	// 若接收到此封包跑的channel與此phy的channel不相同的話,就不接收此封包
		return false;
	
	if(dis<2000){
		if(dis < 1000){
//			return (upon)? upon->RecvFromBellow(pkt,md) :false;
			if( isBusy() ){	// 目前的phy剛好是busy
				g_net_collision ++;	// 發生碰撞加1
				return false;
			}
			if(upon){	// 若有設定上層的話
				_phy_state = PHY_RECV;	// 設定此phy的狀態為「接收」
				_busy_until = g_timer_ticks + 10; //TODO: 要寫上正確的數據 10/1000
				return upon->RecvFromBellow(pkt,md);	// 執行上一層接收資料
			}else
				return false;
		}
	}else{
		return false;	// 因為超出傳送接收的距離,所以傳送失敗
	}

	return false;
}

int Phy802_11 :: GetState()
{
	if( _phy_state != PHY_IDLE ){
		if( g_timer_ticks > _busy_until )
			_phy_state = PHY_IDLE;
	}
	
	return _phy_state;
}

bool Phy802_11 :: isBusy()
{
	if( GetState() == PHY_IDLE )
		return false;
	else
		return true;
}

bool Phy802_11 :: isIdle()
{
	if( GetState() == PHY_IDLE )
		return true;
	else
		return false; 
}
