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

#include "Error.h"
#include "Config.h"
#include "Receiver.hpp"
#include "PacketHandler.h"

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <qobject.h>
#include <qstring.h>
#include <qtimer.h>
#include <qsocketnotifier.h>

#define MAXBUFSIZE 65536
#define IN_BUFSIZE 16384

#define CHECK_TICKTIME 1000

#define IHU_ANONYMOUS "anonymous"

Receiver::Receiver(Rsa *r)
	: rsa(r)
{
	setName("Receiver");

	working = false;
	received = false;

	if ((inputBuffer = (char *)malloc(IN_BUFSIZE))==NULL)
		throw Error(Error::IHU_ERR_MEMORY);
	if ((streamBuffer = (char *)malloc(MAXBUFSIZE))==NULL)
		throw Error(Error::IHU_ERR_MEMORY);

	s = -1;
	notifier = NULL;
	calen = sizeof(ca);

	blowfish = NULL;
	outFile = NULL;

	checkTimer = new QTimer(this);

	reset();
	resetStream();

	connect(checkTimer, SIGNAL(timeout()), this, SLOT(checkConnection()));
}

Receiver::~Receiver(void)
{
	if (inputBuffer)
		free(inputBuffer);
	if (streamBuffer)
		free(streamBuffer);
	if (outFile)
		fclose(outFile);
}

void Receiver::reset()
{
	ihu_refuse = false;
	ihu_reply = false;
	ihu_abort = false;
	connected = false;
	nodecrypt = false;
	active = false;
	total = bytes = 0;
	callerName = IHU_ANONYMOUS;
}

void Receiver::resetStream()
{
	streamLen = 0;
	streamPtr = streamBuffer;
	sync = STREAM_READ_DATA;
}

void Receiver::dump(QString file)
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
		{
			fclose(outFile);
			outFile = NULL;
		}
	}
}

// renyang - 開始接收對方的資料
void Receiver::start(int socket, int proto)
{
//	qWarning("Receiver::start()");
	s = socket;
	protocol = proto;
	// renyang - 連線之後, 可以取得對方的sockaddr_in資料
	::getpeername(s, (struct sockaddr *)&ca, &calen);

	resetStream();
	reset();
	go();

	notifier = new QSocketNotifier(s, QSocketNotifier::Read, this);
	connect(notifier,SIGNAL(activated(int)),this, SLOT(receive()));
	if (received)
	{
		switch(protocol)
		{
			case IHU_UDP:
				receive();
				break;
		}
		emit newSocket(s, protocol, ca);
	}
	// renyang - 開始倒數計時
	checkTimer->start(CHECK_TICKTIME, false);
}

// renyang - 每隔一段時間去判斷是否連接了
void Receiver::checkConnection()
{
	if (received)
	{
		emitSignal(SIGNAL_RINGREPLY);
	}
	if (connected)
	{
		checkTimer->stop();
		emit connectedSignal();
	}
}

void Receiver::close()
{
	if (notifier)
		delete notifier;
	notifier = NULL;
	
	s = -1;

	received = false;
}

void Receiver::end()
{
	close();
	
	checkTimer->stop();
	
	stop();
	
	disableDecrypt();
}

// renyang-TODO - 加入IHU_SCTP的部分
void Receiver::receive()
{
	if (working)
	{
		int rlen = 0;
		
		switch(protocol)
		{
			// renyang - 把接收到的資料放到inputBuffer中
			case IHU_UDP:
				// renyang - 由udp的方式來接收資料
				rlen = ::recvfrom(s, inputBuffer, IN_BUFSIZE, 0, (struct sockaddr *)&ca, &calen);
				break;
			case IHU_TCP:
				// renyang - 由tcp的方式來接收資料
				rlen = ::recv(s, inputBuffer, IN_BUFSIZE, 0);
				break;
		}

		// renyang - 當接收字串大小為0時, 則表示連線結束
		if (rlen > 0)
		{
			bytes += rlen;
			total += rlen;
			active = true;
			putData(inputBuffer, rlen);
		}
		else
		{
			emitSignal(SIGNAL_FINISH);
		}
	}
}

// renyang - 把接收到的資料放到streamBuffer中
void Receiver::putData(char *buffer, int len)
{
	if (outFile)
	{
		// renyang - 把接收到的資料放到outFile中
		fwrite(buffer, 1, len, outFile);
		fflush(outFile);
	}
	
	// renyang - 若剩下的buffer(MAXBUFSIZE - len)小於streamLen，則表示空間不夠, 出現錯誤啦
	// renyang - streamLen可以看成MAXBUFSIZE目前使用到哪裡的index
	if (streamLen > (MAXBUFSIZE - len))
	{
		qWarning("Warning: receiver buffer overloaded.");
		// renyang - 把實際的資料刪成剛好可以放到buffer的大小
		len = MAXBUFSIZE - streamLen;
	}
	memcpy(streamBuffer + streamLen, buffer, len);
	streamLen += len;
	
	// renyang - 資料完成放到streamBuffer中啦
	if (sync == STREAM_READ_DATA)
	{
		sync = STREAM_OK;
		processData();
	}
}

void Receiver::processData()
{
	while (working && (sync != STREAM_READ_DATA))
	{
		switch (sync)
		{
			case STREAM_OK:
				// renyang - 至少要有一個資料, 否則就是資料遺失
				if (streamLen < (HEADER_SYNC_LEN + 1))
				{
					sync = STREAM_MISSING_DATA;
					break;
				}
				// renyang - 比較Header是否有問題
				if (strncmp(streamPtr, HEADER_SYNC_STRING, HEADER_SYNC_LEN) != 0)
				{
					sync = STREAM_OUT_OF_SYNC;
#ifdef IHU_DEBUG
					fprintf(stderr, "OUT OF SYNC (Dump: 0x");
					for(int i=0;i<4;i++)
						fprintf(stderr, "%02x", (unsigned char) streamPtr[i]);
					fprintf(stderr, "\n");
#endif
					break;
				}
				else
				{
					// renyang - 實際封包的大小(內容值是由peer端送過來的)
					unsigned char packetlen = (unsigned char) streamPtr[HEADER_SYNC_LEN];
					int plen = (int) packetlen;
					// renyang - 是大於還是小於啊?
					if (plen > streamLen)
					{
						sync = STREAM_MISSING_DATA;
						break;
					}
					else
					{
						try {
							if (plen >= MIN_PACKET_SIZE)
							{
								Packet *p = new Packet(plen);
								PacketHandler::readPacket(p, streamPtr, plen);
								processPacket(p);
								delete p;
							}
							else
							{
								sync = STREAM_OUT_OF_SYNC;
								break;
							}
							if (plen < streamLen)
							{
								streamPtr = streamPtr + plen;
								streamLen = streamLen - plen;
								sync = STREAM_OK;
							}
							else 
							{
								sync = STREAM_READ_DATA;
								resetStream();
							}
						} catch (Error e)
						{
							emitError(e.getText());
						}
					}
				}
				break;
			case STREAM_OUT_OF_SYNC:
				if (streamLen < 3)
					sync = STREAM_MISSING_DATA;
				else
				{
					do
					{
						streamPtr++;
						streamLen--;
						if (streamLen <= 0)
						{
							sync = STREAM_READ_DATA;
							resetStream();
							break;
						}
						if (strncmp(streamPtr, HEADER_SYNC_STRING, HEADER_SYNC_LEN)==0)
						{
							sync = STREAM_OK;
						}
					} while(sync == STREAM_OUT_OF_SYNC);
				}
				break;
			case STREAM_MISSING_DATA:
				sync = STREAM_READ_DATA;
			case STREAM_READ_DATA:
				memmove(streamBuffer, streamPtr, streamLen);
				streamPtr = streamBuffer;
				break;
		}
	}
}

bool Receiver::processPacket(Packet *p)
{
	switch (p->getInfo())
	{
		case IHU_INFO_CRYPTED_AUDIO:
			if (blowfish)
				p->decrypt(blowfish);
			else
			{
				if (!nodecrypt)
				{
					emitSignal(SIGNAL_KEYREQUEST);
				}
				break;
			}
		case IHU_INFO_AUDIO:
			if (p->getDataLen() > MIN_DATA_SIZE)
			{
				emit newAudioData(p->getData(), p->getDataLen());
			}
			connected = true;
			break;
		case IHU_INFO_NEW_KEY:
			if (p->getDataLen() > MIN_DATA_SIZE)
			{
				char *out;
				int len = rsa->decrypt(p->getData(), p->getDataLen(), &out);
				if (blowfish)
					delete blowfish;
				blowfish = new Blowfish(out, len);
				emitSignal(SIGNAL_NEWKEY);
				free(out);
			}
			break;
		case IHU_INFO_KEY_REQUEST:
			if (p->getDataLen() > MIN_DATA_SIZE)
			{
				rsa->setPeerPublicKey(p->getData(), p->getDataLen());
				emitSignal(SIGNAL_SENDNEWKEY);
			}
			break;
		case IHU_INFO_RING:
			if (p->getDataLen() > MIN_DATA_SIZE)
			{
				ihu_reply = true;
				QString tempName = p->getData(); 
				if (!tempName.isEmpty())
					callerName = tempName;
			}
			emit warning(QString("!! CALL from %1 (%2) !!").arg(getIp()).arg(getCallerName()));
			emitSignal(SIGNAL_RING);
			break;
		case IHU_INFO_ANSWER:
			connected = true;
		case IHU_INFO_RING_REPLY:
			ihu_reply = true;
			if (p->getDataLen() > MIN_DATA_SIZE)
				callerName = p->getData();
			break;
		case IHU_INFO_ERROR:
			ihu_abort = true;
		case IHU_INFO_REFUSE:
			ihu_refuse = true;
		case IHU_INFO_CLOSE:
			emitSignal(SIGNAL_FINISH);
			break;
		case IHU_INFO_INIT:
			emitSignal(SIGNAL_INIT);
		case IHU_INFO_RESET:
			disableDecrypt();
			break;
	}
	return true;
}

void Receiver::emitError(QString text)
{
	emit error(text);
}

void Receiver::enableDecrypt(char *passwd, int len)
{
	disableDecrypt();
	blowfish = new Blowfish(passwd, len);
}

void Receiver::disableDecrypt()
{
	if (blowfish)
		delete blowfish;
	blowfish = NULL;
}

void Receiver::stop()
{
	working = false;
}

void Receiver::go()
{
	working = true;
}

long Receiver::getBytes()
{
	long temp = bytes;
	bytes = 0;
	return temp;
}

long Receiver::getTotal()
{
	return total;
}

QString Receiver::getIp()
{
	return QString(inet_ntoa(ca.sin_addr));
}

QString Receiver::getCallerName()
{
	return callerName;
}

void Receiver::noDecrypt()
{
	nodecrypt = true;
}

bool Receiver::refused()
{
	return ihu_refuse;
}

bool Receiver::aborted()
{
	return ihu_abort;
}

bool Receiver::replied()
{
	return ihu_reply;
}

// renyang - 接收完一個stream
void Receiver::emitSignal(signal_type type)
{
	switch(type)
	{
		case SIGNAL_FINISH:
			emit finishSignal();
			break;
		if (working)
		{
			case SIGNAL_INIT:
				emit initSignal();
				break;
			case SIGNAL_RING:
				emit ringSignal();
				break;
			case SIGNAL_RINGREPLY:
				emit ringReplySignal();
				break;
			case SIGNAL_SENDNEWKEY:
				emit sendNewKey();
				break;
			case SIGNAL_KEYREQUEST:
				emit keyRequest();
				break;
			case SIGNAL_NEWKEY:
				QString text = blowfish->isAsciiKey() ? blowfish->getPass() : "random key";
				emit newKey(text);
				break;
		}
		default:
			break;
	}
}

void Receiver::flush()
{
	stop();
	flushing = true;
}

bool Receiver::isDumping()
{
	bool ret = false;
	if (outFile)
		ret = true;
	return ret;
}

void Receiver::setConnected(bool on)
{
	connected = on;
}

bool Receiver::isConnected()
{
	return connected;
}

void Receiver::setReceived(bool on)
{
	received = on;
}

bool Receiver::isReceived()
{
	return received;
}

bool Receiver::isActive()
{
	bool temp = active;
	active = false;
	return temp;
}
