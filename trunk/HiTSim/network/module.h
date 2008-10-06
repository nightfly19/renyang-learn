
#ifndef __MODULE_H__
#define __MODULE_H__

#include <qobject.h>
#include "network/module.h"
#include "network/MetaData.h"

class Packet;
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
	Module* upon;
	Module* bellow;
//	int id;

public:
	virtual bool Send2Bellow(Packet*,MetaData md=MetaData()) = 0;	
	virtual bool RecvFromUpon(Packet* pkt, MetaData md=MetaData()) = 0; //Call by Upon
	virtual bool RecvFromBellow(Packet*, MetaData) {	return false;	};

};

#endif

