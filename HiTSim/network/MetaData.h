
#ifndef __META_DATA_H__
#define __META_DATA_H__

#include "pos.h"

class MetaData
{
public:
	enum PKT_TYPE{
		PKT_802_11,
		PKT_DSRC
	};

public:
	MetaData( Pos s=Pos(0,0) , Pos r=Pos(0,0), int channel = 0, bool br = false , PKT_TYPE pkt_type = PKT_802_11);

	int Channel(){	return _channel;	}
	bool isBrCast(){	return _brcast;	}
	Pos SndrPos(){	return _sndrPos;	}
	int SndrID(){	return _sndrID;	}
	Pos RecvPos(){	return _recvPos;	}
	int RecvID(){	return _recvID;	}
	int PktType(){	return int(_pkt_type);	};
	
	void setChannel(int c){	_channel = c;	};
	void setBrCast(bool b){	_brcast = b;	};
	void setSndrPos( Pos s){	_sndrPos = s;	};
	void setSndrID(int i){	_sndrID = i;	};
	void setRecvPos( Pos r){	_recvPos = r;	};
	void setRecvID(int i){	_recvID = i;	};
	void setPktType(int i){	_pkt_type = PKT_TYPE(i);	};
	
	double Distance( ); // 計算此封包被傳送與被接收的距離

	void DumpDebug();	// 用來解bug的啦
private: 
	int _channel;	// 記錄此封包是走哪一個channel
	bool _brcast;	// 此封包是否為broadcast的封包
	Pos _sndrPos;	// 記錄傳送此封包時的位置
	int _sndrID;	// 傳送此封包車子的id
	Pos _recvPos;	// 接收到此封包時的位置
	int _recvID;	// 接收到此封包的車子id

	PKT_TYPE _pkt_type;	// 封包形態分為DSRC與802.11

};

#endif 
