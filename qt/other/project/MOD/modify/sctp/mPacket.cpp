

/*
 *  MOD -- Messenger On-the-Drive
 *
 *  Copyright (C) 2006 Kai-Fong Chou - <mod.mapleelpam at gmail.com>
 *
 *  http://mod.0rz.net
 *  http://mod.sf.net
 *
 */
#include "mPacket.h"
#include "Error.h"
#include <string.h>

mPacket::mPacket(int _max_size)
	: max_size(_max_size)
{
	size  = 0;
	// 建立_max_size的buffer
	buf = new char[_max_size];
	if(buf<=0){
		throw Error("memory allocation error");
	}

}

mPacket::mPacket(QString s)
{
	// latin1()我猜是把QString變回一般的c語言中的string吧??
	max_size = size  = strlen(s.latin1()) + 1;
	buf = new char[ max_size];
	if(buf<=0){
		throw Error("memory allocation error");
	}
	memcpy(buf , s.latin1() , max_size);

}

mPacket::~mPacket()
{
	if(buf)
		delete buf;
}

int mPacket::Size()
{
	return size;
}
int mPacket::MaxSize()
{
	return max_size;
}
