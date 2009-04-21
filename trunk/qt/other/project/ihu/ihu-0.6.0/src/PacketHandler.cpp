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
#include "PacketHandler.h"

void PacketHandler::buildPacket(Packet *packet, char *data, int len, char type)
{
	packet->init(data, len);
	packet->setInfo(type);
}

void PacketHandler::buildModePacket(Packet *packet, char *data, int len, char type, char mode)
{
	buildPacket(packet, data, len, type);
	packet->setInfo(mode);
}

void PacketHandler::readPacket(Packet *packet, char *buffer, int len)
{
	packet->fill(buffer, len);
}

int PacketHandler::calculateSize(int dataLen)
{
	int len = dataLen + HEADER_SIZE;
	if (len > MAX_PACKET_SIZE)
	{
		// Just to be sure, but it shouldn't never happen
		fprintf(stderr, "Warning: packet excedeed the maximum size. Please contact author!\n");
	}
	return len;
}

int PacketHandler::calculateCryptedSize(int dataLen)
{
	int align = dataLen % 8;
	if (align)
		dataLen = dataLen + 8 - align;
	return (calculateSize(dataLen));
}
