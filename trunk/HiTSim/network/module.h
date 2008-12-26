
#ifndef __MODULE_H__
#define __MODULE_H__

#include <qobject.h>
#include "network/module.h"
#include "network/MetaData.h"

class Packet;

// Module用來當作網路層每一個的板模
class Module : public QObject
{
Q_OBJECT
public:
	Module ( QObject * parent = 0, const char * name = 0 );
	

public: // set relation
	void setUpon(Module* );
	void setBellow(Module* );
	bool chkUpon(){	return (upon)?true:false;	}
	bool chkBellow(){	return (bellow)?true:false;	}
/*	void setID(int i){	id = i;	};
	int ID(){	return id;	};
	*/
//protected:  //should access by successor
public:
	Module* upon;	// 用來記錄網路架構的上層
	Module* bellow;	// 用來記錄網路架構的下層
//	int id;

public:
	virtual bool Send2Bellow(Packet*,MetaData md=MetaData()) = 0;	
	virtual bool RecvFromUpon(Packet* pkt, MetaData md=MetaData()) = 0; //Call by Upon
	virtual bool RecvFromBellow(Packet*, MetaData) {	return false;	};

};

#endif

