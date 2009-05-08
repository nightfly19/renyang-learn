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

#include <stdio.h>
#include <qapplication.h>
#include "PacketHandler.h"

// renyang - 建立封包
void PacketHandler::buildPacket(Packet *packet, char *data, int len, char type)
{
#ifdef REN_DEBUG
	qWarning("PacketHandler::buildPacket(Packet *packet, char *data, int len, char type)");
#endif
	packet->init(data, len);
	packet->setInfo(type);
}

// renyang - 建立ModePacket封包
// renyang - 感覺這一行跟上面那一個函式是一樣的@@@
void PacketHandler::buildModePacket(Packet *packet, char *data, int len, char type, char mode)
{
#ifdef REN_DEBUG
	qWarning("PacketHandler::buildModePacket(Packet *packet, char *data, int len, char type, char mode)");
#endif
	buildPacket(packet, data, len, type);
	packet->setInfo(mode);
}

// renyang - 把buffer開始長度為len的資料, 複製到packet這一個位址中
// renyang - 感覺不應該叫readPacket, 應該叫writePacket, 因為, 它是把資料寫到自定的封包中
void PacketHandler::readPacket(Packet *packet, char *buffer, int len)
{
#ifdef REN_DEBUG
	qWarning("PacketHandler::readPacket(Packet *packet, char *buffer, int len)");
#endif
	packet->fill(buffer, len);
}

// renyang - 用來計算沒有經過加密的封包大小
int PacketHandler::calculateSize(int dataLen)
{
#ifdef REN_DEBUG
	qWarning(QString("PacketHandler::calculateSize(int %1)").arg(dataLen));
#endif
	int len = dataLen + HEADER_SIZE;
	if (len > MAX_PACKET_SIZE)
	{
		// Just to be sure, but it shouldn't never happen
		fprintf(stderr, "Warning: packet excedeed the maximum size. Please contact author!\n");
	}
	return len;
}

// renyang - 計算有經過加密之後的封包的大小
int PacketHandler::calculateCryptedSize(int dataLen)
{
#ifdef REN_DEBUG
	qWarning(QString("PacketHandler::calculateCryptedSize(int %1)").arg(dataLen));
#endif
	int align = dataLen % 8;
	if (align)
		dataLen = dataLen + 8 - align;
	return (calculateSize(dataLen));
}
