
#ifndef __PACKET_H__
#define __PACKET_H__


#define UNK_SIZE 13

class Packet
{

public:
	Packet();
	Packet(int size);
	Packet(char* data,int size);
	~Packet();

	void init(char* data,int size);
protected:
	char* _data;
	unsigned int _size;
};

class InfoPacket : public Packet
{

public:
	InfoPacket(double x,double y,int c_id,double s = 0);
	InfoPacket(InfoPacket*);
	
	double X() {	return _x;	};
	double Y() {	return _y;	};
	double Speed(){	return _speed;	};
	int cID() {	return _cid;	};

private:
	double _x;
	double _y;
	int _cid;
	double _speed; 
};

#endif

