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

/*
 *  ARCHITECTURE OF THE PACKET
 *
 *  A IHU packet consists of 2 parts:
 *  1) Header
 *  2) Data
 *
 *  THE HEADER
 *
 *  THe header is made up of 3 parts:
 *
 *  a) SYNC Pattern (3 bytes)
 *     The pattern consists of the fixed ASCII string: "IHU"
 *
 *  b) Packet SIZE (1 byte): unsigned char that contains
 *     the size of the packet (min size 5, max size 256)
 *
 *  c) INFO data (1 byte)
 *     It contains those informations (in order):
 *     - OBSOLETE: still used for bakward compatibility (versions < 0.6.x)
 *        Speex mode (3 bits). It can be:
 *        IHU_INFO_MODE_ULTRAWIDE
 *        IHU_INFO_MODE_WIDE
 *        IHU_INFO_MODE_NARROW
 *     - Packet type (5 bits). It can be:
 *        IHU_INFO_RESET 0x0
 *        IHU_INFO_INIT 0x1
 *        IHU_INFO_AUDIO 0x2
 *        IHU_INFO_CRYPTED_AUDIO 0x3
 *        IHU_INFO_KEY_REQUEST 0x4
 *        IHU_INFO_NEW_KEY 0x5
 *        IHU_INFO_ANSWER 0x6
 *        IHU_INFO_CLOSE 0x7
 *        IHU_INFO_REFUSE 0x8
 *        IHU_INFO_RING 0x3f
 *        IHU_INFO_RING_REPLY 0x3e
 *
 *  d) Data LENGTH (1 byte): unsigned char that contains
 *     the size of the following DATA part (min 0, max 250)
 *
 *  THE DATA
 *
 *  This part of the packet is of variable size (min 0, max 250 bytes)
 *  and it contains the data to be trasmitted.
 *  It can be absent (if the packet doesn't need to transmit any data) 
 *  or, can be audio data or keys for encpryption.
 *  The type of data transmitted is specified in the INFO header
 *
 */

#include "Error.h"
#include "Packet.h"

Packet::Packet(int packetSize)
{
	size = packetSize;
	if ((packet = (char *)malloc(size))==NULL)
		throw Error(Error::IHU_ERR_MEMORY);
	memset(packet, 0x0, size);
}

Packet::~Packet(void)
{
	free(packet);
}

void Packet::init(char *data, int len)
{
	strncpy(packet, HEADER_SYNC_STRING, HEADER_SYNC_LEN);
	packet[HEADER_SYNC_LEN] = (unsigned char) size;
	info = packet + HEADER_SYNC_LEN + 1; // Info
	dataLen = (unsigned char *) (packet + HEADER_SYNC_LEN + 2);
	*dataLen = (unsigned char) len;
	if (len > 0)
	{
		dataPtr = packet + HEADER_SIZE;
		memcpy(dataPtr, data, len);
	}
	resetInfo();
}

void Packet::resetInfo()
{
	*info = 0x0;
}

void Packet::setInfo(char param)
{
	*info |= param;
}

char Packet::getInfo()
{
	return (*info & 0x3f);
}

bool Packet::isInfo(char param)
{
	return (getInfo() & param);
}

char Packet::getSeq()
{
	return (*info & 0xc0);
}

void Packet::fill(char *buffer, int len)
{
	memcpy(packet, buffer, len);
	dataPtr = packet + HEADER_SIZE;
	dataLen = (unsigned char *) dataPtr - 1;
	info = dataPtr - 2;
}

void Packet::crypt(Blowfish *blowfish)
{
	int len = *dataLen % DATA_BLOCK_SIZE;
	if (len)
		len = *dataLen + DATA_BLOCK_SIZE - len;
	else len = *dataLen;
	blowfish->encrypt(dataPtr, len);
}

void Packet::decrypt(Blowfish *blowfish)
{
	if (dataLen > 0)
		blowfish->decrypt(dataPtr, size - HEADER_SIZE);
}

int Packet::getSize()
{
	return size;
}

char * Packet::getPacket()
{
	return packet;
}

char * Packet::getData()
{
	return dataPtr;
}

int Packet::getDataLen()
{
	return (int) *dataLen;
}
