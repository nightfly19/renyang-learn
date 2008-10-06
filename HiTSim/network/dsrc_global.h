
#ifndef __DSRC_GLOBAL_H__
#define __DSRC_GLOBAL_H__
#include <qcolor.h>
enum DSRC_CHANNELS{
	CH_NORMAL1 = 0,
	CH_NORMAL2,
	CH_NORMAL3,
	CH_NORMAL4, 
	CH_CONTROL,
	CH_WARNG1,
	CH_WARNG2,
	CH_MAX
};

int inline ChDsrc2Number( int ch )
{
	switch( ch ){
	case CH_WARNG1:
		return 0;
	case CH_NORMAL1:
		return 1;
	case CH_NORMAL2:
		return 2;
	case CH_CONTROL:
		return 3;
	case CH_NORMAL3:
		return 4;
	case CH_NORMAL4: 
		return 5;
	case CH_WARNG2:
		return 6; 
	default:
		return -1;
	}
	return -1;
}

extern QColor ChannelColors[CH_MAX];

#endif
