
#include "global.h"

unsigned int gMaxNodeID = 1;
int gCarAmount = 0;
unsigned int gStartPosition = 20;
unsigned int gEndPosition = 0;
int gViewLocation = 199;

int gMaxVelocity = 100;
int gMinVelocity = 60;
int gFrictionFactorNormal = 300;
int gFrictionFactorEmerg = 600;
int gSafeDistance = 3;

int gHumanResponseTime = 400; //msec //要用的
int gSystemHandleTime = 20; //要用的
/* never used */
int gSensorDetectionTime = 40;
int gMachineWorkTime = 40;
/* never used */

int gTimeSlot = 13;
double gTr = 0.3;

double gTimeRatio = 1.00;
bool gCollisionDetection = false;

double g_timer_ticks = 0;

/* timer gap */
unsigned int g_car_timer_gap = 100; // 1/1000 section
unsigned int g_gps_timer_gap = 50; // 1/1000 section
unsigned int g_gps_update_time = 500;
unsigned int g_ticks_timer_gap = 1; // 1/1000 section


/* 數據 收集 */
unsigned int g_net_collision = 0;
unsigned int g_car_collision = 0;
unsigned int g_car_too_closed = 0;

unsigned int g_simulate_until = 720; //section

