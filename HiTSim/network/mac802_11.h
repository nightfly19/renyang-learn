
#ifndef __MAC_802_11_H__
#define __MAC_802_11_H__

#define MAC_Subtype_RTS         0x0B
#define MAC_Subtype_CTS         0x0C
#define MAC_Subtype_ACK         0x0D
#define MAC_Subtype_Data        0x00

#include "mac.h"

class Mac802_11 : public Mac
{
public:
	Mac802_11( QObject * parent = 0, const char * name = 0 );

};

#endif

