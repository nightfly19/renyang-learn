
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
	
	double Distance( );

	void DumpDebug();
private: 
	int _channel;
	bool _brcast;
	Pos _sndrPos;
	int _sndrID;
	Pos _recvPos;
	int _recvID;

	PKT_TYPE _pkt_type;

};

#endif 
