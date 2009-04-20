

/*
 *  IHU -- I Hear U, easy VoIP application using Speex and Qt
 *
 *  Copyright (C) 2003-2005 M. Trotta - <mrotta@users.sourceforge.net>
 *
 *  http://ihu.sourceforge.net
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111, USA.
 */

/*
 *  MOD -- Messenger On-the-Drive
 *
 *  Copyright (C) 2006 Kai-Fong Chou - <mod.mapleelpam at gmail.com>
 *
 *  http://mod.0rz.net
 *  http://mod.sf.net
 *
 */

#ifndef _PACKET_H
#define _PACKET_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Blowfish.h"

#define MAX_PACKET_SIZE 255

#define HEADER_SIZE 6
#define HEADER_SYNC_STRING "DRTA"
#define HEADER_SYNC_LEN 3

#define DATA_BLOCK_SIZE 8

#define DRTA_INFO_RESET 0x0
#define DRTA_INFO_INIT 0x1
#define DRTA_INFO_AUDIO 0x2
#define DRTA_INFO_CRYPTED_AUDIO 0x3
#define DRTA_INFO_KEY_REQUEST 0x4
#define DRTA_INFO_NEW_KEY 0x5
#define DRTA_INFO_ANSWER 0x6
#define DRTA_INFO_CLOSE 0x7
#define DRTA_INFO_REFUSE 0x8
#define DRTA_INFO_MSG 0x9

#define DRTA_INFO_RING 0x3f
#define DRTA_INFO_RING_REPLY 0x3e

#define DRTA_INFO_MODE_ULTRAWIDE 0xc0
#define DRTA_INFO_MODE_WIDE 0x80
#define DRTA_INFO_MODE_NARROW 0x40

class Packet{
public:
	Packet(int);
	~Packet(void);
	void init(char*, int);
	void fill(char*, int);
	void crypt(Blowfish *);
	void decrypt(Blowfish *);
	char *getPacket();
	char getMode();
	int getSize();
	char *getData();
	int getDataLen();
	void resetInfo();
	void setInfo(char);
	char getInfo();
	bool isInfo(char);
private:
	char *packet;
	char *dataPtr;
	int size;
	unsigned char *dataLen;
	char *info;
};

#endif
