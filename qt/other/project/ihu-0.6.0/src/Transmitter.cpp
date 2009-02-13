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

#include "Transmitter.hpp"
#include "Config.h"
#include "Error.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include "PacketHandler.h"

#define RINGTIME 3000

Transmitter::Transmitter(Rsa *r) : rsa(r)
{
	setName("Transmitter");
	salen = sizeof(sa);
	outFile = NULL;
	blowfish = NULL;
	crypted = false;
	timer = new QTimer(this);
	reset();
	connect(timer, SIGNAL(timeout()), this, SLOT(sendRing()));
}

Transmitter::~Transmitter(void)
{
	if (outFile)
		fclose(outFile);
}

void Transmitter::reset()
{
	s = -1;
	total = bytes = 0;
	working = false;
	ringing = false;
	active = false;
	tx = false;
}

void Transmitter::init(int prot)
{
	protocol = prot;
	
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;

	reset();
}

void Transmitter::setMyName(QString name)
{
	myName = name;
}

void Transmitter::dump(QString file)
{
	if (!file.isEmpty())
	{
		outFile = fopen(file.ascii(), "ab");
		if (outFile==NULL)
			throw Error(QString("%1: %2").arg(file).arg(strerror(errno)));
	}
	else
	{
		if (outFile)
			fclose(outFile);
		outFile = NULL;
	}
}

void Transmitter::go()
{
	working = true;
}

void Transmitter::stop()
{
	working = false;
}

void Transmitter::newConnection(int socket, struct sockaddr_in ca, int prot)
{
	init(prot);
	
	switch (prot)
	{
		case IHU_UDP:
			if ((::connect(socket, (struct sockaddr *)&ca, salen))==-1)
				throw Error(strerror(errno));
			break;
	}
	::getpeername(socket, (struct sockaddr *)&sa, &salen);
	start(socket);
}

void Transmitter::start(int socket)
{
	s = socket;
	go();
}

int Transmitter::call(QString host, int port, int prot)
{
	init(prot);

	int sd = -1;
	
	sa.sin_port = htons(port);
	
	if (inet_aton(host.ascii(), &sa.sin_addr) == 0)
	{
		struct hostent *he;

		he = gethostbyname(host.ascii());
		if (he == NULL)
			throw Error(tr("can't resolve ") + host.ascii());
		sa.sin_addr = *(struct in_addr *) he ->h_addr;
	}
	
	int type = 0;
	
	switch(protocol)
	{
		case IHU_UDP:
			type = SOCK_DGRAM;
			break;
		case IHU_TCP:
			type = SOCK_STREAM;
			break;
		default:
			throw Error("unknown protocol");
	}
	
	if ((sd = socket(AF_INET, type, 0)) == -1)
		throw Error(tr("can't initalize socket (") + strerror(errno)+ tr(")"));
	
	if ((::connect(sd, (struct sockaddr *)&sa, sizeof(sa)))==-1)
		throw Error(strerror(errno));
	
	start(sd);
	
	return sd;
}

void Transmitter::end()
{
	stop();
	
	timer->stop();
	
	s = -1;
}

void Transmitter::enableCrypt(char *passwd, int len)
{
	disableCrypt();
	blowfish = new Blowfish(passwd, len);
	sendResetPacket();
	crypted = true;
}

void Transmitter::disableCrypt()
{
	if (blowfish)
		delete blowfish;
	blowfish = NULL;
	crypted = false;
}

void Transmitter::sendPacket(Packet *p)
{
	int snt = 0;
	if ((s != -1) && working)
	{
		snt = ::send(s, p->getPacket(), p->getSize(), MSG_NOSIGNAL);
		if (snt <= 0)
		{
			emitSignal(SIGNAL_FINISH);
		}
		else
		{
			bytes += snt;
			total += snt;
			active = true;
		}
	}
	if (outFile) {
		fwrite(p->getPacket(), 1, p->getSize(), outFile);
		fflush(outFile);
	}
}

void Transmitter::prepare()
{
	sendInitPacket();
	tx = true;
	emitSignal(SIGNAL_START);
}

void Transmitter::ring(bool on)
{
	if (on)
	{
		tx = false;
		ringing = true;
		sendRing();
	}
	else
	{
		ringing = false;
	}
}

void Transmitter::sendRing()
{
	if (ringing)
	{
		timer->start(RINGTIME, true);
		sendRingPacket();
		emitSignal(SIGNAL_RINGMESSAGE);
	}
	else
	{
		try
		{
			prepare();
		}
		catch (Error e)
		{
			emitError(e.getText());
		}
	}
}

void Transmitter::sendAudioPacket(char *data, int len)
{
//	qWarning("Transmitter::SendAudioPacket");
	if (tx)
	{
		int size;
		char type = IHU_INFO_AUDIO;
		try
		{
			if (blowfish)
			{
				size = PacketHandler::calculateCryptedSize(len);
				type = IHU_INFO_CRYPTED_AUDIO;
			}
			else
				size = PacketHandler::calculateSize(len);
			Packet *p = new Packet (size);
			PacketHandler::buildModePacket(p, data, len, type, IHU_INFO_MODE_ULTRAWIDE);
			if (blowfish)
				p->crypt(blowfish);
			sendPacket(p);
			delete p;
		}
		catch (Error e)
		{
			emitError(e.getText());
		}
	}
}

void Transmitter::emitError(QString text)
{
	emit error(text);
}

void Transmitter::sendSpecialPacket(char *data, int len, char type)
{
//	qWarning("Transmitter::SendSpecialPacket");
	try
	{
		int size = PacketHandler::calculateSize(len);
		Packet *p = new Packet (size);
		PacketHandler::buildPacket(p, data, len, type);
		sendPacket(p);
		delete p;
	}
	catch (Error e)
	{
		emitError(e.getText());
	}
}

void Transmitter::sendNewPacket(char *data, int len, char type)
{
//	qWarning("Transmitter::SendNewPacket");
	try
	{
		int size = PacketHandler::calculateSize(len);
		Packet *p = new Packet (size);
		PacketHandler::buildModePacket(p, data, len, IHU_INFO_MODE_ULTRAWIDE, type);
		sendPacket(p);
		delete p;
	}
	catch (Error e)
	{
		emitError(e.getText());
	}
}

void Transmitter::sendNamePacket(bool special, char type)
{
//	qWarning("Transmitter::SendNamePacket");
	int dataLen = 0;
	char *dataPtr = NULL;
	if (!myName.isEmpty())
	{
		dataLen = myName.length()+1;
		dataPtr = (char *) myName.ascii();
	}
	if (special)
		sendSpecialPacket(dataPtr, dataLen, type);
	else
		sendNewPacket(dataPtr, dataLen, type);
}

void Transmitter::answer()
{
	prepare();
	sendAnswerPacket();
}

void Transmitter::sendAnswerPacket()
{
	sendNamePacket(false, IHU_INFO_ANSWER);
}

void Transmitter::sendRingPacket()
{
	sendNamePacket(false, IHU_INFO_RING);
}

void Transmitter::sendRingReplyPacket()
{
	sendNamePacket(true, IHU_INFO_RING_REPLY);
}

void Transmitter::sendRefusePacket()
{
	sendSpecialPacket(NULL, 0, IHU_INFO_REFUSE);
}

void Transmitter::sendClosePacket()
{
	sendSpecialPacket(NULL, 0, IHU_INFO_CLOSE);
}

void Transmitter::sendResetPacket()
{
	sendSpecialPacket(NULL, 0, IHU_INFO_RESET);
}

void Transmitter::sendInitPacket()
{
	sendNewPacket(NULL, 0, IHU_INFO_INIT);
}

void Transmitter::sendErrorPacket()
{
	sendNewPacket(NULL, 0, IHU_INFO_ERROR);
}

void Transmitter::sendKeyPacket()
{
	char *out;
	int keylen = rsa->getPeerPublicKeyLen();
	try
	{
		if (keylen > 0)
		{
			if (blowfish)
			{
				int len = rsa->encrypt(blowfish->getPass(), blowfish->getPassLen(), &out);
				sendSpecialPacket(out, len, IHU_INFO_NEW_KEY);
				free(out);
			}
		}
		else
		{
			sendResetPacket();
		}
	}
	catch (Error e)
	{
		emitError(e.getText());
	}
}

void Transmitter::sendKeyRequestPacket()
{
	int keylen = rsa->getMyPublicKeyLen();
	char *public_key = rsa->getMyPublicKey();
	sendSpecialPacket(public_key, keylen, IHU_INFO_KEY_REQUEST);
}

long Transmitter::getBytes()
{
	long temp = bytes;
	bytes = 0;
	return temp;
}

long Transmitter::getTotal()
{
	return total;
}

QString Transmitter::getIp()
{
	return QString(inet_ntoa(sa.sin_addr));
}

bool Transmitter::isWorking()
{
	return working;
}

bool Transmitter::isCrypted()
{
	return crypted;
}

bool Transmitter::isDumping()
{
	bool ret = false;
	if (outFile)
		ret = true;
	return ret;
}

bool Transmitter::isActive()
{
	bool temp = active;
	active = false;
	return temp;
}

void Transmitter::emitSignal(signal_type type)
{
	switch(type)
	{
		if (working)
		{
			case SIGNAL_FINISH:
				emit finishSignal();
				break;
			case SIGNAL_START:
				emit startSignal();
				break;
			case SIGNAL_RINGMESSAGE:
				emit ringMessage();
				break;
		}
		default:
			break;
	}
}
