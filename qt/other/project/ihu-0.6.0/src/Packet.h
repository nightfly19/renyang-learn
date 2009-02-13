/*
 *  IHU -- I Hear U, easy VoIP application using Speex and Qt
 *
 *  Copyright (C) 2003-2008 Matteo Trotta - <mrotta@users.sourceforge.net>
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

#ifndef PACKET_HPP
#define PACKET_HPP

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Blowfish.h"

#define MAX_PACKET_SIZE 255
#define MIN_PACKET_SIZE 6

#define HEADER_SIZE 6
#define HEADER_SYNC_STRING "IHU"
#define HEADER_SYNC_LEN 3

#define MIN_DATA_SIZE 0
#define MAX_DATA_SIZE 250

#define DATA_BLOCK_SIZE 8

#define IHU_INFO_RESET 0x0
#define IHU_INFO_INIT 0x1
#define IHU_INFO_AUDIO 0x2
#define IHU_INFO_CRYPTED_AUDIO 0x3
#define IHU_INFO_KEY_REQUEST 0x4
#define IHU_INFO_NEW_KEY 0x5
#define IHU_INFO_ANSWER 0x6
#define IHU_INFO_CLOSE 0x7
#define IHU_INFO_REFUSE 0x8
#define IHU_INFO_ERROR 0x9
#define IHU_INFO_RING 0x3f
#define IHU_INFO_RING_REPLY 0x3e

#define IHU_INFO_MODE_ULTRAWIDE 0xc0
#define IHU_INFO_MODE_WIDE 0x80
#define IHU_INFO_MODE_NARROW 0x40

class Packet{
public:
	Packet(int);
	~Packet(void);
	void init(char*, int);
	void fill(char*, int);
	void crypt(Blowfish *);
	void decrypt(Blowfish *);
	char *getPacket();
	char getSeq();
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
