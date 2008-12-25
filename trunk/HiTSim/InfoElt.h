
#ifndef __INFO_ELEMENT__
#define __INFO_ELEMENT__

// 用來記錄要顯示在每一台車子上接收到封包的list的資料結構

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

	int sndr_id; // 傳送此封包的車子的編號
	double sndr_x; // 此封包傳送時傳送端的車子的x位置
	double sndr_y; // 此封包傳送時傳送端的車子的y位置
	double sndr_speed; // 此封包傳送時,傳送端車子的速度

	double recv_time; // 接收到的時間
	double recv_x; // 接收到時本車的x位置
	double recv_y; // 接收到時本車的y位置

	double distance;
};

#endif

