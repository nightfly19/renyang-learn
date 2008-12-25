
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "network/packet.h"

Packet::Packet()
	: _data(NULL)
	, _size(0)
{
	// do nothing
}
Packet::Packet(int size)
	: _data(NULL)
	, _size(size)
{
	// do nothing
}
Packet::Packet(char* data,int size)
	: _data(data)
	, _size(size)
{
	// do nothing
}

Packet::~Packet()
{
	if(_data)
		delete _data;
}

void Packet :: init(char* data, int size)
{
	_size = size;
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
	// do nothing
}
