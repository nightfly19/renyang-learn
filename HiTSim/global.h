
#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <qcolor.h>
//#include "Car.h"

#define LOGO_RTTI 1025		// RTTI : Run-Time Type Information
#define CAR_RTTI 1026
#define ROAD_RTTI 1027


#define CAR_MX 1000

#define CAR_WIDTH	5*10	//4公尺
#define CAR_HEIGHT	27	//2公尺

extern unsigned int gMaxNodeID;
extern int gCarAmount;
extern unsigned int gStartPosition;
extern unsigned int gEndPosition;
extern int gViewLocation;

extern int gMaxVelocity;
extern int gMinVelocity;
extern int gFrictionFactorNormal;
extern int gFrictionFactorEmerg;
extern int gSafeDistance;

extern int gHumanResponseTime;
extern int gSensorDetectionTime;
extern int gSystemHandleTime;
extern int gMachineWorkTime;
extern double gTr;

extern int gTimeSlot;

extern double gTimeRatio;


extern bool gCollisionDetection;

extern double g_timer_ticks;

extern unsigned int g_car_timer_gap;
extern unsigned int g_gps_timer_gap;
extern unsigned int g_gps_update_time;
extern unsigned int g_ticks_timer_gap;


extern unsigned int g_net_collision;
extern unsigned int g_car_collision;
extern unsigned int g_car_too_closed;
extern unsigned int g_simulate_until; //section
inline double DoubleAbs(double b)
{
	if( b < 0) return -b;
	else    return b;
}

inline double KMHR_2_MS(double i)
{
	return i*1000.0 / (60.0*60.0);
}
#endif

