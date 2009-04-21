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
	connect( receiver, SIGNAL(finishSignal()), this, SLOT(stopCall()) );
	connect( receiver, SIGNAL(error(QString)), this, SLOT(abortCall(QString)) );
	connect( receiver, SIGNAL(warning(QString)), this, SLOT(warning(QString)) );
	connect( receiver, SIGNAL(message(QString)), this, SLOT(warning(QString)) );
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
	return id;
}

void Call::stopCall()
{
//	qWarning("Call::stopCall()");
	stop();
	warning("Closing communication...");
}

void Call::stop()
{
//	qWarning("Call::stop()");
	if (active)
	{
		active = false;
		recording = false;
		sendRing(false);
		stopTimer->start(STOP_TIME, true);
		transmitter->end();
		receiver->end();
	}
}

void Call::close()
{
//	qWarning("Call::close()");
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
//	qWarning("Call::abortCall()");
	aborted = true;
	emit abortSignal(id, text);
}

void Call::call(QString host, int port, int prot)
{
	try
	{
		readyFrames = 0;
		sd = transmitter->call(host, port, prot);
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

void Call::start(int socket, int protocol)
{
//	qWarning("Call::start()");
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
//	qWarning("Call::newConnection()");
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

void Call::connected()
{
//	qWarning("Call::connected()");
	sendRing(false);
	emit connectedSignal(id);
	emit warning(QString("Connected with %1 (%2)").arg(getCallerName()).arg(getCallerIp()));
}

void Call::error()
{
	aborted = true;
	transmitter->sendErrorPacket();
	stop();
}

void Call::answer()
{
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
//	qWarning("Call::end()");
	if (receiver->isReceived() && !receiver->isConnected())
		transmitter->sendRefusePacket();
	else
		transmitter->sendClosePacket();
	stop();
}

void Call::sendKey()
{
	transmitter->sendKeyPacket();
}

void Call::sendKeyRequest()
{
//	qWarning("Call::sendKeyRequest()");
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
	emit newKeySignal(id, text);
}

void Call::enableDecrypt(char *passwd, int len)
{
	receiver->enableDecrypt(passwd, len);
}

void Call::enableRandomCrypt(int len)
{
	char key[len];
	for (int i=0; i<len; i++)
		key[i] = (char)((rand()%256)-128);
	enableCrypt(key, len);
}

void Call::enableCrypt(char *passwd, int len)
{
	try
	{
		transmitter->enableCrypt(passwd, len);
	}
	catch (Error e)
	{
		 abortCall(e.getText());
	}

}

void Call::send(char *data, int len)
{
	if (recording && !muteRec)
		transmitter->sendAudioPacket(data, len);
}

void Call::message(QString text)
{
	warning(text);
}

void Call::warning(QString text)
{
	emit warningSignal(id, text);
}

void Call::playRing()
{
//	qWarning("Call::playRing()");
	transmitter->sendRingReplyPacket();
	if (!mutePlay)
		emit ringSignal(id);
}

void Call::playInit()
{
//	qWarning("Call::playInit()");
	if (!mutePlay)
		emit initSignal(id);
}

void Call::ringMessage()
{
	QString text;
	if (receiver->replied())
		text = QString("Ringing %1 (%2)").arg(transmitter->getIp()).arg(receiver->getCallerName());
	else
		text = QString("Contacting %1...").arg(transmitter->getIp());
	warning(text);
}

void Call::sendRing(bool on)
{
	transmitter->ring(on);
}

void Call::decodeAudioData(char *buf, int len)
{
//	qWarning("Call::decodeAudioData()");
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
//	qWarning(QString("Call::playData() - readyFrames: %1 len %2").arg(readyFrames).arg(len));;
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
//	qWarning(QString("Call::mixData() - Balance: %1").arg(balance, 2, 'f', 1 ));
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
//	qWarning(QString("Call::putData() - readyFrames: %1 len: %2").arg(readyFrames).arg(len));;
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
	transmitter->disableCrypt();
}

void Call::disableDecrypt()
{
	receiver->disableDecrypt();
}

void Call::startRecorder()
{
	recording = true;
}

void Call::muteRecorder(bool on)
{
	muteRec = on;
}

void Call::mutePlayer(bool on)
{
	mutePlay = on;
}

void Call::dumpRXStream(QString filename)
{
	receiver->dump(filename);
}

void Call::dumpTXStream(QString filename)
{
	transmitter->dump(filename);
}

bool Call::isDumpingRX()
{
	return receiver->isDumping();
}

bool Call::isDumpingTX()
{
	return transmitter->isDumping();
}

bool Call::isRXActive()
{
	return receiver->isActive();
}

bool Call::isTXActive()
{
	return transmitter->isActive();
}

bool Call::isFree()
{
	return callFree;
}

QString Call::getCallerName()
{
	return receiver->getCallerName();
}

QString Call::getCallerIp()
{
	return receiver->getIp();
}

long Call::getTraffic()
{
	long total = transmitter->getBytes() + receiver->getBytes();
	return total;
}

long Call::getTotalRX()
{
	return receiver->getTotal();
}

long Call::getTotalTX()
{
	return transmitter->getTotal();
}

long Call::getTotal()
{
	long total = transmitter->getTotal() + receiver->getTotal();
	return total;
}

void Call::setMyName(QString myName)
{
	transmitter->setMyName(myName);
}

void Call::updateFrames(int frames)
{
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
	return recording;
}
