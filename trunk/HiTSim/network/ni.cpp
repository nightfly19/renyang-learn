
#include "network/ni.h"

NI :: NI ( QObject * parent, const char * name )
	: QObject(parent,name)
	, _ni_state(NI_IDLE)
{
	// do nothing
}
