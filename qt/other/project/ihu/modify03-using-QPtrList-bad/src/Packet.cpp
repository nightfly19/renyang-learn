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
// renyang - 記錄封包的大小
 *  b) Packet SIZE (2 byte): unsigned short int that contains
 *     the size of the packet (min size 8, max size 1024)
 *
 *  c) INFO data (1 byte)
 *     It contains those informations (in order):
 *     - OBSOLETE: still used for bakward compatibility (versions < 0.6.x)
 *        Speex mode (2 bits). It can be:
 *        IHU_INFO_MODE_ULTRAWIDE
 *        IHU_INFO_MODE_WIDE
 *        IHU_INFO_MODE_NARROW
 *     - Packet type (6 bits). It can be:
 *        IHU_INFO_RESET 0x0
 *        IHU_INFO_INIT 0x1
 *        IHU_INFO_AUDIO 0x2
 *        IHU_INFO_CRYPTED_AUDIO 0x3
 *        IHU_INFO_KEY_REQUEST 0x4
 *        IHU_INFO_NEW_KEY 0x5
 *        IHU_INFO_ANSWER 0x6
 *        IHU_INFO_CLOSE 0x7
 *        IHU_INFO_REFUSE 0x8
 *        IHU_INFO_ERROR 0x9
 // renyang-modify - 傳送的資料
 *        IHU_INFO_VIDEO 0xa
 // renyang-modify - 當server端webcam有問題時, 會傳送以這一個為開頭的特殊封包
 *        IHU_INFO_VIDEO_ERROR 0xb
 // renyang-modify - 表示server端傳送image frame結尾啦
 *        IHU_INFO_VIDEO_END 0xc
 // renyang-modify - 由client要求下一個image divide(我把一個image frame切成很多個image divide)
 *        IHU_INFO_VIDEO_NEXT 0xd
 // renyang-modify - 由client端要求server傳送image frame
 *        IHU_INFO_VIDEO_REQUEST 0xe
 // renyang-modify - 用來當作confirm, 來確認其它沒有用的ip是否還是通的
 *        IHU_INFO_VIDEO_CONFIRM 0xf
 *        IHU_INFO_RING 0x3f
 *        IHU_INFO_RING_REPLY 0x3e
 *
 *  d) Data LENGTH (2 byte): unsigned short int that contains
 *     the size of the following DATA part (min 0, max 1016)
 *
 *  THE DATA
 *
 *  This part of the packet is of variable size (min 0, max 1016 bytes)
 *  and it contains the data to be trasmitted.
 *  It can be absent (if the packet doesn't need to transmit any data) 
 *  or, can be audio data or keys for encpryption.
 *  The type of data transmitted is specified in the INFO header
 *
 */

#include "Error.h"
#include "Packet.h"

// renyang - 建立一個封包, 準備要拿來放資料
Packet::Packet(int packetSize)
{
#ifdef REN_DEBUG
	qWarning(QString("Packet::Packet(int %1)").arg(packetSize));
#endif
	size = packetSize;
	// renyang - 動態建立Packet的大小
	if ((packet = (char *)malloc(size))==NULL)
		throw Error(Error::IHU_ERR_MEMORY);
	memset(packet, 0x0, size);
}

Packet::~Packet()
{
#ifdef REN_DEBUG
	qWarning("Packet::~Packet()");
#endif
	free(packet);
}

// renyang - 初始化這一個封包, 它的資料內容與封包長度
void Packet::init(char *data, int len)
{
#ifdef REN_DEBUG
	qWarning(QString("Packet::init(char *data, int %1)").arg(len));
#endif
	// renyang - 初始化封包的header
	strncpy(packet, HEADER_SYNC_STRING, HEADER_SYNC_LEN);
	packetLen = (unsigned short int *) (packet + HEADER_SYNC_LEN);
	*packetLen = (unsigned short int) size;
	info = packet + HEADER_SYNC_LEN + 2; // Info
	dataLen = (unsigned short int *) (packet + HEADER_SYNC_LEN + 3);
	*dataLen = (unsigned short int) len;
	if (len > 0)
	{
		dataPtr = packet + HEADER_SIZE;
		// renyang - 把資料copy到自訂的封包中
		memcpy(dataPtr, data, len);
	}
	resetInfo();
}

void Packet::resetInfo()
{
#ifdef REN_DEBUG
	qWarning("Packet::resetInfo()");
#endif
	*info = 0x0;
}

// renyang - 設定這一個封包的型態
void Packet::setInfo(char param)
{
#ifdef REN_DEBUG
	qWarning("Packet::setInfo(char param)");
#endif
	*info |= param;
}

char Packet::getInfo()
{
#ifdef REN_DEBUG
	qWarning("Packet::getInfo()");
#endif
	return (*info & 0x3f);
}

bool Packet::isInfo(char param)
{
#ifdef REN_DEBUG
	qWarning("Packet::isInfo(char param)");
#endif
	return (getInfo() & param);
}

char Packet::getSeq()
{
#ifdef REN_DEBUG
	qWarning("Packet::getSeq()");
#endif
	return (*info & 0xc0);
}

// renyang - 依資料建立Packet
void Packet::fill(char *buffer, int len)
{
#ifdef REN_DEBUG
	qWarning(QString("Packet::fill(char *buffer, int %1)").arg(len));
#endif
	memcpy(packet, buffer, len);
	dataPtr = packet + HEADER_SIZE;
	dataLen = (unsigned short int *) dataPtr - 1;
	info = dataPtr - 3;
}

void Packet::crypt(Blowfish *blowfish)
{
#ifdef REN_DEBUG
	qWarning("Packet::crypt(Blowfish *blowfish)");
#endif
	int len = *dataLen % DATA_BLOCK_SIZE;
	if (len)
		len = *dataLen + DATA_BLOCK_SIZE - len;
	else len = *dataLen;
	blowfish->encrypt(dataPtr, len);
}

void Packet::decrypt(Blowfish *blowfish)
{
#ifdef REN_DEBUG
	qWarning("Packet::decrypt(Blowfish *blowfish)");
#endif
	if (dataLen > 0)
		blowfish->decrypt(dataPtr, size - HEADER_SIZE);
}

int Packet::getSize()
{
#ifdef REN_DEBUG
	qWarning("Packet::getSize()");
#endif
	return size;
}

char * Packet::getPacket()
{
#ifdef REN_DEBUG
	qWarning("Packet::getPacket()");
#endif
	return packet;
}

char * Packet::getData()
{
#ifdef REN_DEBUG
	qWarning("Packet::getData()");
#endif
	return dataPtr;
}

int Packet::getDataLen()
{
#ifdef REN_DEBUG
	qWarning("Packet::getDataLen()");
#endif
	return (int) *dataLen;
}
