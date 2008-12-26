
#include "network/module.h"

Module :: Module( QObject * parent, const char * name)
	: QObject(parent,name)
	, upon(NULL), bellow(NULL) 
//	, id(0)
{
	// do nothing
}


void Module :: setUpon(Module* u)
{
	upon = u;
}
void Module :: setBellow(Module* b)
{
	bellow = b;
}


/*
bool Module :: Send2Bellow(Packet*,int channel,bool brcast)
{
	return false;
}*/

