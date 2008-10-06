
#ifndef __INFO_ELEMENT__
#define __INFO_ELEMENT__


class Car;
class InfoPacket;
class InfoElement
{

public:
	InfoElement(InfoPacket* , Car* );

	int SndrID(){	return sndr_id;	};
	double SndrX(){	return sndr_x;	};
	double SndrY(){	return sndr_y;	};
	double SndrSpeed(){	return sndr_speed;	};

	double RecvTime(){	return recv_time;	};
	double RecvX(){	return recv_x;	};
	double RecvY(){	return recv_y;	};

private:

	int sndr_id;
	double sndr_x;
	double sndr_y;
	double sndr_speed;

	double recv_time;
	double recv_x;
	double recv_y;

	double distance;
};

#endif

