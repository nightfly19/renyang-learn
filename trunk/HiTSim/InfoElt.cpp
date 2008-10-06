
#include "global.h"
#include "InfoElt.h"
#include "Car.h"
#include "network/packet.h"

InfoElement :: InfoElement(InfoPacket* ip, Car* car)
{
	sndr_id = ip -> cID();
	sndr_x = ip -> X();
	sndr_y = ip -> Y();
	sndr_speed  = ip -> Speed();

	recv_time = g_timer_ticks;
	recv_x = car -> realX();
	recv_y = car -> realY();
}
