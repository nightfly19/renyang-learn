
#ifndef __PACKET_H__
#define __PACKET_H__


#define UNK_SIZE 13

// 封包

class Packet
{

public:
	Packet();
	Packet(int size);
	Packet(char* data,int size);
	~Packet();

	void init(char* data,int size);
protected:
	char* _data; // 資料
	unsigned int _size; // 封包大小
};

// 有包含車子資訊的封包

class InfoPacket : public Packet
{

public:
	InfoPacket(double x,double y,int c_id,double s = 0); // 初始化封包
	InfoPacket(InfoPacket*); // 封包複製
	
	double X() {	return _x;	};
	double Y() {	return _y;	};
	double Speed(){	return _speed;	};
	int cID() {	return _cid;	};

private:
	double _x; // 車子的位置
	double _y; // 車子的位置
	int _cid; // 車子的編號
	double _speed; // 車子的速度
};

#endif

