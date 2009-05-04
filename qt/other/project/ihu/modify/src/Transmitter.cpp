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
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::Transmitter(Rsa *r):rsa(r)"));
#endif
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
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::~Transmitter(void)"));
#endif
	if (outFile)
		fclose(outFile);
}

void Transmitter::reset()
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::reset()"));
#endif
	s = -1;
	total = bytes = 0;
	working = false;
	ringing = false;
	active = false;
	tx = false;
}

// renyang - 設定真實的socket protocal, 並且把自定的protocal(IHU_TCP或IHU_UDP)存在protocol
void Transmitter::init(int prot)
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::init(%1)").arg(prot));
#endif
	protocol = prot;
	
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;

	reset();
}

void Transmitter::setMyName(QString name)
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::setMyName(%1)").arg(name));
#endif
	myName = name;
}

void Transmitter::dump(QString file)
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::dump(%1)").arg(file));
#endif
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
#ifdef REN_DEBUG
	qWarning(tr("Transmitter::go()"));
#endif
	working = true;
}

void Transmitter::stop()
{
#ifdef REN_DEBUG
	qWarning(tr("Transmitter::stop()"));
#endif
	working = false;
}

// renyang - 建立一個新連線
// renyang-TODO - 以後可能要在這裡加入IHU_SCTP吧
void Transmitter::newConnection(int socket, struct sockaddr_in ca, int prot)
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::newConnection(int %1, struct sockaddr_in ca, int %2)").arg(socket).arg(prot));
#endif
	init(prot);
	
	switch (prot)
	{
		case IHU_UDP:
			// renyang - 原本udp不需要用到connect這一個函式, 但是要用好像還是可以
			// renyang - 這樣才能改成one-to-one的udp型態
			// renyang - 這樣才可以直接對socket就可以傳送資料了
			if ((::connect(socket, (struct sockaddr *)&ca, salen))==-1)
				throw Error(strerror(errno));
			break;
	}
	// renyang - 設定給TCP使用的啦
	::getpeername(socket, (struct sockaddr *)&sa, &salen);
	start(socket);
}

// renyang - 開始要傳送資料啦
void Transmitter::start(int socket)
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::start(int %1)").arg(socket));
#endif
	s = socket;
	go();
}

// renyang - 要打電話出去, 要連到server端
// renyang-TODO - 要加上sctp的部分
int Transmitter::call(QString host, int port, int prot)
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::call(QString %1,int %2, int %3)").arg(host).arg(port).arg(prot));
#endif
	// renyang - 設定真實的socket protocal, 並且把自定的protocal(IHU_TCP或IHU_UDP)存在protocol
	init(prot);

	int sd = -1;
	
	// renyang - 設定port號(也是對方的服務port號)
	sa.sin_port = htons(port);
	
	// renyang - 當回傳是1時, 表示轉換成功. 回傳是0時, 表示轉換失敗
	if (inet_aton(host.ascii(), &sa.sin_addr) == 0)
	{
		struct hostent *he;

		// renyang - 把tw.yahoo.com轉換成ip
		he = gethostbyname(host.ascii());
		if (he == NULL)
			throw Error(tr("can't resolve ") + host.ascii());
		// renyang - 轉出來的部分直接就是network address啦
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
			throw Error(tr("unknown protocol"));
	}
	
	// renyang - 建立clinet端的socket
	if ((sd = ::socket(AF_INET, type, 0)) == -1)
		throw Error(tr("can't initalize socket (") + strerror(errno)+ tr(")"));
	
	// renyang - 此socket是用來控制連到server端用的
	if ((::connect(sd, (struct sockaddr *)&sa, sizeof(sa)))==-1)
		throw Error(strerror(errno));
	
	// renyang - 要儲存要使用的socket. s=sd
	start(sd);
	
	return sd;
}

void Transmitter::end()
{
#ifdef REN_DEBUG
	qWarning("Transmitter::end()");
#endif
	stop();
	
	timer->stop();
	
	s = -1;
}

void Transmitter::enableCrypt(char *passwd, int len)
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::enableCrypt(char *passwd, int %1)").arg(len));
#endif
	disableCrypt();
	blowfish = new Blowfish(passwd, len);
	sendResetPacket();
	crypted = true;
}

void Transmitter::disableCrypt()
{
#ifdef REN_DEBUG
	qWarning("Transmitter::disableCrypt()");
#endif
	if (blowfish)
		delete blowfish;
	blowfish = NULL;
	crypted = false;
}

// renyang - 傳送封包
void Transmitter::sendPacket(Packet *p)
{
#ifdef REN_DEBUG
	qWarning("Transmitter::sendPacket(Packet *p)");
#endif
	int snt = 0;
	if ((s != -1) && working)
	{
		// renyang - 把資料透過::send()傳送出去
		// renyang - 就算是udp也可以使用::send, 因為是connect型態的udp
		snt = ::send(s, p->getPacket(), p->getSize(), MSG_NOSIGNAL);
		// renyang - 傳送失敗
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
	// renyang - 是否把資料存到檔案中
	if (outFile) {
		fwrite(p->getPacket(), 1, p->getSize(), outFile);
		fflush(outFile);
	}
}

void Transmitter::prepare()
{
#ifdef REN_DEBUG
	qWarning("void Transmitter::prepare()");
#endif
	sendInitPacket();
	tx = true;
	emitSignal(SIGNAL_START);
}

// renyang - 請傳送出去, 叫對方送出鈴聲
void Transmitter::ring(bool on)
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::ring(bool %1)").arg(on));
#endif
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
#ifdef REN_DEBUG
	qWarning("Transmitter::sendRing()");
#endif
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
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::sendAudioPacket(char *data,int %1)").arg(len));
#endif
	if (tx)
	{
		int size;
		// renyang - 了解傳送出去的是什麼型態的資料
		char type = IHU_INFO_AUDIO;
		try
		{
			// renyang - 是否要經過加密
			if (blowfish)
			{
				size = PacketHandler::calculateCryptedSize(len);
				type = IHU_INFO_CRYPTED_AUDIO;
			}
			else
			{
				size = PacketHandler::calculateSize(len);
			}
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
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::emitError(QString %1)").arg(text));
#endif
	emit error(text);
}

// renyang - 建立要送出去的封包
void Transmitter::sendSpecialPacket(char *data, int len, char type)
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::SendSpecialPacket(char %1,int %2, char %3)").arg(data).arg(len).arg(type));
#endif
	try
	{
		// renyang - 計算這一個封包所需要的大小
		int size = PacketHandler::calculateSize(len);
		Packet *p = new Packet (size);
		// renyang - 建立封包
		PacketHandler::buildPacket(p, data, len, type);
		// renyang - 傳送封包
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
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::SendNewPacket(char %1,int %2, char %3)").arg(data).arg(len).arg(type));
#endif
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
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::SendNamePacket(bool %1, char %2)").arg(special).arg(type));
#endif
	int dataLen = 0;
	char *dataPtr = NULL;
	if (!myName.isEmpty())
	{
		dataLen = myName.length()+1;
		dataPtr = (char *) myName.ascii();
	}
	// renyang - special應該是表是說是否之前有傳送過, 像是傳送鈴聲, 就是重複的封包(還是特殊的封包???)
	if (special)
		sendSpecialPacket(dataPtr, dataLen, type);
	else
		sendNewPacket(dataPtr, dataLen, type);
}

void Transmitter::answer()
{
#ifdef REN_DEBUG
	qWarning("Transmitter::answer()");
#endif
	prepare();
	sendAnswerPacket();
}

void Transmitter::sendAnswerPacket()
{
#ifdef REN_DEBUG
	qWarning("Transmitter::sendAnswerPacket()");
#endif
	sendNamePacket(false, IHU_INFO_ANSWER);
}

void Transmitter::sendRingPacket()
{
#ifdef REN_DEBUG
	qWarning("Transmitter::sendRingPacket()");
#endif
	sendNamePacket(false, IHU_INFO_RING);
}

// renyang - 傳送重覆答鈴聲
void Transmitter::sendRingReplyPacket()
{
#ifdef REN_DEBUG
	qWarning("Transmitter::sendRingReplyPacket()");
#endif
	sendNamePacket(true, IHU_INFO_RING_REPLY);
}

void Transmitter::sendRefusePacket()
{
#ifdef REN_DEBUG
	qWarning("Transmitter::sendRefusePacket()");
#endif
	sendSpecialPacket(NULL, 0, IHU_INFO_REFUSE);
}

void Transmitter::sendClosePacket()
{
#ifdef REN_DEBUG
	qWarning("Transmitter::sendClosePacket()");
#endif
	sendSpecialPacket(NULL, 0, IHU_INFO_CLOSE);
}

void Transmitter::sendResetPacket()
{
#ifdef REN_DEBUG
	qWarning("Transmitter::sendResetPacket()");
#endif
	sendSpecialPacket(NULL, 0, IHU_INFO_RESET);
}

void Transmitter::sendInitPacket()
{
#ifdef REN_DEBUG
	qWarning("Transmitter::sendInitPacket()");
#endif
	sendNewPacket(NULL, 0, IHU_INFO_INIT);
}

void Transmitter::sendErrorPacket()
{
#ifdef REN_DEBUG
	qWarning("Transmitter::sendErrorPacket()");
#endif
	sendNewPacket(NULL, 0, IHU_INFO_ERROR);
}

void Transmitter::sendKeyPacket()
{
#ifdef REN_DEBUG
	qWarning("Transmitter::sendKeyPacket()");
#endif
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
#ifdef REN_DEBUG
	qWarning("Transmitter::sendKeyRequestPacket()");
#endif
	int keylen = rsa->getMyPublicKeyLen();
	char *public_key = rsa->getMyPublicKey();
	sendSpecialPacket(public_key, keylen, IHU_INFO_KEY_REQUEST);
}

long Transmitter::getBytes()
{
#ifdef REN_DEBUG
	qWarning("Transmitter::getBytes()");
#endif
	long temp = bytes;
	bytes = 0;
	return temp;
}

long Transmitter::getTotal()
{
#ifdef REN_DEBUG
	qWarning("Transmitter::getTotal()");
#endif
	return total;
}

QString Transmitter::getIp()
{
#ifdef REN_DEBUG
	qWarning("Transmitter::getIp()");
#endif
	return QString(inet_ntoa(sa.sin_addr));
}

bool Transmitter::isWorking()
{
#ifdef REN_DEBUG
	qWarning("Transmitter::isWorking()");
#endif
	return working;
}

bool Transmitter::isCrypted()
{
#ifdef REN_DEBUG
	qWarning("Transmitter::isCrypted()");
#endif
	return crypted;
}

bool Transmitter::isDumping()
{
#ifdef REN_DEBUG
	qWarning("Transmitter::isDumping()");
#endif
	bool ret = false;
	if (outFile)
		ret = true;
	return ret;
}

bool Transmitter::isActive()
{
#ifdef REN_DEBUG
	qWarning("Transmitter::isActive()");
#endif
	bool temp = active;
	active = false;
	return temp;
}

// renyang - 傳送訊號
void Transmitter::emitSignal(signal_type type)
{
#ifdef REN_DEBUG
	qWarning("Transmitter::emitSignal(signal_type type)");
#endif
	switch(type)
	{
		if (working)
		{
			case SIGNAL_FINISH:
				// renyang - Call會執行stopCall()
				emit finishSignal();
				break;
			case SIGNAL_START:
				// renyang - Call::startRecorder()
				emit startSignal();
				break;
			case SIGNAL_RINGMESSAGE:
				// renyang - Call::ringMessage()
				emit ringMessage();
				break;
		}
		default:
			break;
	}
}
