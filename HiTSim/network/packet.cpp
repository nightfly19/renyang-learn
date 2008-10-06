
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "network/packet.h"

Packet::Packet()
	: _data(NULL)
	, _size(0)
{
	init(NULL,0);
}
Packet::Packet(int size)
	: _data(NULL)
	, _size(size)
{
	init(NULL,size);
}
Packet::Packet(char* data,int size)
{
	init(data,size);
}

Packet::~Packet()
{
	if(_data)
		delete _data;
}

void Packet :: init(char* data, int size)
{
	_size = size;
	/*
	if ((_data = (char *)malloc(_size))==NULL)
		exit(1);
	*/
//	memset(_data, 0x0, _size); 
	_data = new char[_size];
	if( !_data )
		exit(1);

	if(_data)
		strncpy( _data, data, size );
}

InfoPacket::InfoPacket(double x,double y,int cid,double s)
	: Packet()
	, _x(x)
	, _y(y)
	, _cid(cid)
	, _speed(s)
{
/*
	char data[2];
	data[0] = x;
	data[1] = y;

	init(data,2);
	*/
}


InfoPacket::InfoPacket(InfoPacket* pkt)
	: _x( pkt-> X() )
	, _y( pkt-> Y() )
	, _cid( pkt -> cID() )
	, _speed( pkt -> Speed() )
{
}
