
#include <math.h>

#include "network/phy.h"
#include "network/packet.h" 
#include "global.h"

Phy:: Phy( QObject * parent, const char * name )
	: Module(parent,name)
	, _phy_state( PHY_IDLE )
	, _busy_until(0)
{
}

