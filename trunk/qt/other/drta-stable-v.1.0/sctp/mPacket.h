

/*
 *  MOD -- Messenger On-the-Drive
 *
 *  Copyright (C) 2006 Kai-Fong Chou - <mod.mapleelpam at gmail.com>
 *
 *  http://mod.0rz.net
 *  http://mod.sf.net
 *
 */

#ifndef _M_PACKET_H
#define _M_PACKET_H

#include <qstring.h>

class mPacket
{

	public:
		mPacket(int _max_size);
		mPacket(QString s);
		~mPacket();

		int Size();
		int MaxSize();
		void setSize(int s){	size = s;	};

		char* getBufPtr(){	return buf;	};

	private:
		int size;
		int max_size;
		char* buf;

};

#endif
