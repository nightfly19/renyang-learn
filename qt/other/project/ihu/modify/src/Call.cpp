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

#include "Config.h"
#include "Error.h"
#include "Call.hpp"

#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define STOP_TIME 2000

#define MAXBUFSIZE 65536

Call::Call(int callId, QString myName)
{
#ifdef IHU_DEBUG
	qWarning(QString("Call::Call(int %1, QString %2)").arg(callId).arg(myName));
#endif
	id = callId;
	sd = -1;

	dec_state = NULL;
	dec_state = speex_decoder_init(&speex_uwb_mode);
	speex_bits_init(&bits);

	int enh = 1;
	speex_decoder_ctl(dec_state, SPEEX_SET_ENH, &enh);
	speex_decoder_ctl(dec_state, SPEEX_GET_FRAME_SIZE, &frame_size);

	if ((outBuffer = (float *) malloc(frame_size*4*sizeof(float)))==NULL)
		throw Error(Error::IHU_ERR_MEMORY);
	if ((soundBuffer = (float *) malloc(MAXBUFSIZE*sizeof(float)))==NULL)
		throw Error(Error::IHU_ERR_MEMORY);
	readyFrames = 0;

	rsa = new Rsa(RSA_STRENGTH);
	transmitter = new Transmitter(rsa);
	receiver = new Receiver(rsa);

	stopTimer = new QTimer(this);

	active = false;
	muteRec = false;
	mutePlay = false;
	callFree = true;
	aborted = false;
	recording = false;

	transmitter->setMyName(myName);

	srand(time(NULL));

	connect( receiver, SIGNAL(newSocket(int,int,struct sockaddr_in)), this, SLOT(newConnection(int,int,struct sockaddr_in)) );
	connect( receiver, SIGNAL(keyRequest()), this, SLOT(sendKeyRequest()) );
	connect( receiver, SIGNAL(sendNewKey()), this, SLOT(sendKey()) );
	connect( receiver, SIGNAL(newKey(QString)), this, SLOT(receivedNewKey(QString)) );
	// renyang - 沒有再接收到client端傳送過來的訊息, 結束此Call
	connect( receiver, SIGNAL(finishSignal()), this, SLOT(stopCall()) );
	connect( receiver, SIGNAL(error(QString)), this, SLOT(abortCall(QString)) );
	connect( receiver, SIGNAL(warning(QString)), this, SLOT(warning(QString)) );
	connect( receiver, SIGNAL(message(QString)), this, SLOT(warning(QString)) );
	// renyang - 對方接受通話, 或是本地端接受通話
	connect( receiver, SIGNAL(connectedSignal()), this, SLOT(connected()) );
	connect( receiver, SIGNAL(ringSignal()), this, SLOT(playRing()) );
	connect( receiver, SIGNAL(initSignal()), this, SLOT(playInit()) );
	connect( receiver, SIGNAL(newAudioData(char*, int)), this, SLOT(decodeAudioData(char*, int)) );
	connect( receiver, SIGNAL(ringReplySignal()), transmitter, SLOT(sendRingReplyPacket()) );

	connect( transmitter, SIGNAL(ringMessage()), this, SLOT(ringMessage()) );
	connect( transmitter, SIGNAL(finishSignal()), this, SLOT(stopCall()) );
	connect( transmitter, SIGNAL(error(QString)), this, SLOT(abortCall(QString)) );
	connect( transmitter, SIGNAL(message(QString)), this, SLOT(message(QString)) );
	connect( transmitter, SIGNAL(startSignal()), this, SLOT(startRecorder()) );

	connect( stopTimer, SIGNAL(timeout()), this, SLOT(close()) );
}

Call::~Call()
{
#ifdef IHU_DEBUG
	qWarning("Call::~Call()");
#endif
	if (dec_state)
		speex_decoder_destroy(dec_state);
	speex_bits_destroy(&bits);
	if (soundBuffer)
		free(soundBuffer);
	if (outBuffer)
		free(outBuffer);
	delete rsa;
	delete receiver;
	delete transmitter;
}

int Call::getId()
{
#ifdef IHU_DEBUG
	qWarning("Call::getId()");
#endif
	return id;
}

// renyang - 記錄要結束通訊
void Call::stopCall()
{
#ifdef IHU_DEBUG
	qWarning("Call::stopCall()");
#endif
	stop();
	warning("Closing communication...");
}

void Call::stop()
{
#ifdef IHU_DEBUG
	qWarning("Call::stop()");
#endif
	if (active)
	{
		active = false;
		recording = false;
		sendRing(false);
		// renyang - 隔一段時間之後, 才正真close掉
		stopTimer->start(STOP_TIME, true);
		transmitter->end();
		receiver->end();
	}
}

void Call::close()
{
#ifdef IHU_DEBUG
	qWarning("Call::close()");
#endif
	if (sd != -1)
		::close(sd);
	sd = -1;
	callFree = true;

	readyFrames = 0;

	QString text;
	if (aborted)
		text = QString("Call aborted (an error occurred)");
	else
	if (receiver->aborted())
		text = QString("Call aborted by peer (an error occurred)");
	else
	if (receiver->refused())
		text = QString("%1 refused the call.").arg(receiver->getCallerName());
	else
	if (receiver->getTotal() > 0)
		text = QString("Communication with %1 closed.").arg(receiver->getCallerName());
	else
		text = QString("%1 rejected the call.").arg(receiver->getIp());
	warning(text);
	emit cancelCallSignal(id);
}

void Call::abortCall(QString text)
{
#ifdef IHU_DEBUG
	qWarning("Call::abortCall()");
#endif
	aborted = true;
	emit abortSignal(id, text);
}

// renyang - 要撥打出去的電話(ip address, port, protype)
void Call::call(QString host, int port, int prot)
{
#ifdef IHU_DEBUG
	qWarning(QString("Call::call(QString %1, int %2, int %3)").arg(host).arg(port).arg(prot));
#endif
	try
	{
		readyFrames = 0;
		// renyang - 由transmitter連線到server端, 回傳表示peer端的socket
		sd = transmitter->call(host, port, prot);
		// renyang - 接收端開始接收由對方回傳的資料並且判斷對方的動作
		// renyang - 開始查看是否有新的資料由peer端傳送過來
		receiver->start(sd, prot);
		sendRing(true);
		callFree = false;
		active = true;
		aborted = false;
	}
	catch (Error e)
	{
		aborted = true;
		e.setCallId(id);
		throw e;
	}
}

// renyang - 建立一個網路連線
// renyang - protocol:IHU_TCP或IHU_UDP
void Call::start(int socket, int protocol)
{
#ifdef IHU_DEBUG
	qWarning(QString("Call::start(int %1,int %2)").arg(socket).arg(protocol));
#endif
	try
	{
		readyFrames = 0;
		receiver->setReceived(true);
		receiver->start(socket, protocol);
		callFree = false;
		active = true;
		aborted = false;
	}
	catch (Error e)
	{
		abortCall(e.getText());
	}
}

void Call::newConnection(int socketd, int protocol, struct sockaddr_in sa)
{
#ifdef IHU_DEBUG
	qWarning(QString("Call::newConnection(int %1,int %2,struct sockaddr_in sa)").arg(socketd).arg(protocol));
#endif
	try
	{
		sd = socketd;
		transmitter->newConnection(sd, sa, protocol);
		emit warning(QString("Incoming connection from %1!").arg(receiver->getIp()));
	}
	catch (Error e)
	{
		abort();
	}
}

// renyang - 連線成功(不論是本地端接受通話或是遠端接受通話)
void Call::connected()
{
#ifdef IHU_DEBUG
	qWarning("Call::connected()");
#endif
	// renyang - 連線成功就不需要ring啦
	sendRing(false);
	// renyang - 告知Phone，目前此Call id連線成功啦
	emit connectedSignal(id);
	emit warning(QString("Connected with %1 (%2)").arg(getCallerName()).arg(getCallerIp()));
}

void Call::error()
{
#ifdef IHU_DEBUG
	qWarning("Call::error()");
#endif
	aborted = true;
	transmitter->sendErrorPacket();
	stop();
}

void Call::answer()
{
#ifdef IHU_DEBUG
	qWarning("Call::answer()");
#endif
	try
	{
		transmitter->answer();
		receiver->setConnected(true);
	}
	catch (Error e)
	{
		e.setCallId(id);
		throw e;
	}
}

void Call::end()
{
#ifdef IHU_DEBUG
	qWarning("Call::end()");
#endif
	if (receiver->isReceived() && !receiver->isConnected())
		transmitter->sendRefusePacket();
	else
		transmitter->sendClosePacket();
	stop();
}

void Call::sendKey()
{
#ifdef IHU_DEBUG
	qWarning("Call::sendKey()");
#endif
	transmitter->sendKeyPacket();
}

void Call::sendKeyRequest()
{
#ifdef IHU_DEBUG
	qWarning("Call::sendKeyRequest()");
#endif
	if (transmitter->isWorking())
	{
		transmitter->sendKeyRequestPacket();
		if (!transmitter->isCrypted())
			emit cryptedSignal(id);
	}
	else
	{
		if (active)
		{
			abortCall(QString("stream is crypted but the decryption key is not available."));
		}
	}
}

void Call::receivedNewKey(QString text)
{
#ifdef IHU_DEBUG
	qWarning(QString("Call::receivedNewKey(QString %1)").arg(text));
#endif
	emit newKeySignal(id, text);
}

void Call::enableDecrypt(char *passwd, int len)
{
#ifdef IHU_DEBUG
	qWarning(QString("Call::enableDecrypt(char %1, int %2)").arg(passwd).arg(len));
#endif
	receiver->enableDecrypt(passwd, len);
}

void Call::enableRandomCrypt(int len)
{
#ifdef IHU_DEBUG
	qWarning(QString("Call::enableRandomCrypt(int %1)").arg(len));
#endif
	char key[len];
	for (int i=0; i<len; i++)
		key[i] = (char)((rand()%256)-128);
	enableCrypt(key, len);
}

void Call::enableCrypt(char *passwd, int len)
{
#ifdef IHU_DEBUG
	qWarning(QString("Call::enableCrypt(char %1, int %2)").arg(passwd).arg(len));
#endif
	try
	{
		transmitter->enableCrypt(passwd, len);
	}
	catch (Error e)
	{
		 abortCall(e.getText());
	}

}

// renyang - 把資料由網路傳送出去
void Call::send(char *data, int len)
{
#ifdef IHU_DEBUG
	qWarning(QString("Call::send(char *data, int %1)").arg(len));
#endif
	// renyang - 有記錄, 且不是靜音
	if (recording && !muteRec)
		transmitter->sendAudioPacket(data, len);
}

void Call::message(QString text)
{
#ifdef IHU_DEBUG
	qWarning(QString("Call::message(QString %1)").arg(text));
#endif
	warning(text);
}

void Call::warning(QString text)
{
#ifdef IHU_DEBUG
	qWarning(QString("Call::warning(QString %1)").arg(text));
#endif
	emit warningSignal(id, text);
}

void Call::playRing()
{
#ifdef IHU_DEBUG
	qWarning("Call::playRing()");
#endif
	transmitter->sendRingReplyPacket();
	if (!mutePlay)
		emit ringSignal(id);
}

void Call::playInit()
{
#ifdef IHU_DEBUG
	qWarning("Call::playInit()");
#endif
	if (!mutePlay)
		emit initSignal(id);
}

void Call::ringMessage()
{
#ifdef IHU_DEBUG
	qWarning("Call::ringMessage()");
#endif
	QString text;
	if (receiver->replied())
		text = QString("Ringing %1 (%2)").arg(transmitter->getIp()).arg(receiver->getCallerName());
	else
		text = QString("Contacting %1...").arg(transmitter->getIp());
	warning(text);
}

void Call::sendRing(bool on)
{
#ifdef IHU_DEBUG
	qWarning(QString("Call::sendRing(bool %1)").arg(on));
#endif
	transmitter->ring(on);
}

void Call::decodeAudioData(char *buf, int len)
{
#ifdef IHU_DEBUG
	qWarning("Call::decodeAudioData()");
#endif
	if (dec_state)
	{
		speex_bits_read_from(&bits, buf, len);
		if (speex_decode(dec_state, &bits, outBuffer) < 0)
		{
			emit warning("Warning: wrong decryption key or stream corrupted!");
			disableDecrypt();
		}
		else
		{
			putData(outBuffer, frame_size);
		}
	}
}

bool Call::playData(float *buf, int len)
{
#ifdef IHU_DEBUG
	qWarning(QString("Call::playData() - readyFrames: %1 len %2").arg(readyFrames).arg(len));;
#endif
	bool ret = false;
	if (readyFrames >= len)
	{
		if (!mutePlay)
		{
			for (int i=0; i<len; i++)
				buf[i] = soundBuffer[i];
			ret = true;
		}
		updateFrames(len);
	}
	return ret;
}

bool Call::mixData(float *buf, int len, float balance)
{
#ifdef IHU_DEBUG
	qWarning(QString("Call::mixData() - Balance: %1").arg(balance, 2, 'f', 1 ));
#endif
	bool ret = false;
	if (readyFrames >= len)
	{
		if (!mutePlay)
		{
			for (int i=0; i<len; i++)
				buf[i] = (buf[i]*(1.f-balance)) + (soundBuffer[i]*balance);
			ret = true;
		}
		updateFrames(len);
	}
	return ret;
}

void Call::putData(float *buf, int len)
{
#ifdef IHU_DEBUG
	qWarning(QString("Call::putData() - readyFrames: %1 len: %2").arg(readyFrames).arg(len));;
#endif
	if (len < (MAXBUFSIZE - readyFrames))
	{
		memcpy(soundBuffer+readyFrames, buf, len*sizeof(float));
		readyFrames += len;
	}
	else
	{
		qWarning(QString("Warning: Call %1 buffer overloaded").arg(id));
	}
}

void Call::disableCrypt()
{
#ifdef IHU_DEBUG
	qWarning("Call::disableCrypt()");
#endif
	transmitter->disableCrypt();
}

void Call::disableDecrypt()
{
#ifdef IHU_DEBUG
	qWarning("Call::disableDecrypt()");
#endif
	receiver->disableDecrypt();
}

void Call::startRecorder()
{
#ifdef IHU_DEBUG
	qWarning("Call::startRecorder()");
#endif
	recording = true;
}

void Call::muteRecorder(bool on)
{
#ifdef IHU_DEBUG
	qWarning(QString("Call::muteRecorder(bool %1)").arg(on));
#endif
	muteRec = on;
}

void Call::mutePlayer(bool on)
{
#ifdef IHU_DEBUG
	qWarning(QString("Call::mutePlayer(bool %1)").arg(on));
#endif
	mutePlay = on;
}

void Call::dumpRXStream(QString filename)
{
#ifdef IHU_DEBUG
	qWarning(QString("Call::dumpRXStream(QString %1)").arg(filename));
#endif
	receiver->dump(filename);
}

void Call::dumpTXStream(QString filename)
{
#ifdef IHU_DEBUG
	qWarning(QString("Call::dumpTXStream(QString %1)").arg(filename));
#endif
	transmitter->dump(filename);
}

bool Call::isDumpingRX()
{
#ifdef IHU_DEBUG
	qWarning("Call::isDumpingRX()");
#endif
	return receiver->isDumping();
}

bool Call::isDumpingTX()
{
#ifdef IHU_DEBUG
	qWarning("Call::isDumpingTX()");
#endif
	return transmitter->isDumping();
}

bool Call::isRXActive()
{
#ifdef IHU_DEBUG
	qWarning("Call::isRXActive()");
#endif
	return receiver->isActive();
}

bool Call::isTXActive()
{
#ifdef IHU_DEBUG
	qWarning("Call::isTXActive()");
#endif
	return transmitter->isActive();
}

bool Call::isFree()
{
#ifdef IHU_DEBUG
	qWarning("Call::isFree()");
#endif
	return callFree;
}

QString Call::getCallerName()
{
#ifdef IHU_DEBUG
	qWarning("Call::getCallerName()");
#endif
	return receiver->getCallerName();
}

// renyang - 取得對方的ip address
QString Call::getCallerIp()
{
#ifdef IHU_DEBUG
	qWarning("Call::getCallerIp()");
#endif
	return receiver->getIp();
}

long Call::getTraffic()
{
#ifdef IHU_DEBUG
	qWarning("Call::getTraffic()");
#endif
	long total = transmitter->getBytes() + receiver->getBytes();
	return total;
}

long Call::getTotalRX()
{
#ifdef IHU_DEBUG
	qWarning("Call::getTotalRX()");
#endif
	return receiver->getTotal();
}

long Call::getTotalTX()
{
#ifdef IHU_DEBUG
	qWarning("Call::getTotalTX()");
#endif
	return transmitter->getTotal();
}

long Call::getTotal()
{
#ifdef IHU_DEBUG
	qWarning("Call::getTotal()");
#endif
	long total = transmitter->getTotal() + receiver->getTotal();
	return total;
}

void Call::setMyName(QString myName)
{
#ifdef IHU_DEBUG
	qWarning(QString("Call::setMyName(QString %1)").arg(myName));
#endif
	transmitter->setMyName(myName);
}

void Call::updateFrames(int frames)
{
#ifdef IHU_DEBUG
	qWarning(QString("Call::updateFrames(int %1)").arg(frames));
#endif
	readyFrames -= frames;
	if (readyFrames < 0)
		readyFrames = 0;
	if (readyFrames > 0)
	{
		memcpy(soundBuffer, soundBuffer+frames, sizeof(float)*readyFrames);
	}
}

bool Call::isRecording()
{
#ifdef IHU_DEBUG
	qWarning("Call::isRecording()");
#endif
	return recording;
}
