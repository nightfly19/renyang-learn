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
#ifdef REN_DEBUG
	qWarning(QString("Call::Call(int %1, QString %2)").arg(callId).arg(myName));
#endif
	id = callId;
	sd = -1;

	dec_state = NULL;
	dec_state = speex_decoder_init(&speex_uwb_mode);
	// renyang - Initializes and allocates resources for a SpeexBits struct
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

	// renyang-modify - 接收由receiver傳送上來的peer address
	connect (receiver,SIGNAL(SignalgetIps(QStringList)),this,SLOT(SlotgetIps(QStringList)));
	// renyang-modify - 當Receiver接收到與之前的primary不同時, 要求改變primary address
	connect (receiver,SIGNAL(setPrimaddrSignal(QString)),this,SLOT(setPrimaddr(QString)));
	// renyang-modify - end

	connect( transmitter, SIGNAL(ringMessage()), this, SLOT(ringMessage()) );
	connect( transmitter, SIGNAL(finishSignal()), this, SLOT(stopCall()) );
	connect( transmitter, SIGNAL(error(QString)), this, SLOT(abortCall(QString)) );
	connect( transmitter, SIGNAL(message(QString)), this, SLOT(message(QString)) );
	connect( transmitter, SIGNAL(startSignal()), this, SLOT(startRecorder()) );

	connect( stopTimer, SIGNAL(timeout()), this, SLOT(close()) );

}

Call::~Call()
{
#ifdef REN_DEBUG
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
#ifdef REN_DEBUG
	qWarning("Call::getId()");
#endif
	return id;
}

// renyang - 記錄要結束通訊
void Call::stopCall()
{
#ifdef REN_DEBUG
	qWarning("Call::stopCall()");
#endif
	stop();
	warning("Closing communication...");
}

void Call::stop()
{
#ifdef REN_DEBUG
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
#ifdef REN_DEBUG
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
#ifdef REN_DEBUG
	qWarning("Call::abortCall()");
#endif
	aborted = true;
	emit abortSignal(id, text);
}

// renyang - 要撥打出去的電話(ip address, port, protype)
void Call::call(QString host, int port, int prot)
{
#ifdef REN_DEBUG
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
		// renyang - 送出響鈴的封包
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
#ifdef REN_DEBUG
	qWarning(QString("Call::start(int %1,int %2)").arg(socket).arg(protocol));
#endif
	try
	{
		readyFrames = 0;
		// renyang - 設定本端是接收端(就是等待別人打電話過來的啦)
		qWarning("1111");
		receiver->setReceived(true);
		qWarning("2222");
		// renyang - 開始定時的去查看是否有新的資料可以接收
		receiver->start(socket, protocol);
		qWarning("3333");
		callFree = false;
		active = true;
		aborted = false;
	}
	catch (Error e)
	{
		abortCall(e.getText());
	}
}

// renyang - 建立一個傳送的protocol
// renyang - 若是tcp, one-to-one sctp, 其struct sockaddr_in sa用不到
void Call::newConnection(int socketd, int protocol, struct sockaddr_in sa)
{
#ifdef REN_DEBUG
	qWarning(QString("Call::newConnection(int %1,int %2,struct sockaddr_in sa)").arg(socketd).arg(protocol));
#endif
	try
	{
		sd = socketd;
		// renyang - 建立一個專門給Transmitter使用的socket
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
#ifdef REN_DEBUG
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
#ifdef REN_DEBUG
	qWarning("Call::error()");
#endif
	aborted = true;
	transmitter->sendErrorPacket();
	stop();
}

void Call::answer()
{
#ifdef REN_DEBUG
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
#ifdef REN_DEBUG
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
#ifdef REN_DEBUG
	qWarning("Call::sendKey()");
#endif
	transmitter->sendKeyPacket();
}

void Call::sendKeyRequest()
{
#ifdef REN_DEBUG
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
#ifdef REN_DEBUG
	qWarning(QString("Call::receivedNewKey(QString %1)").arg(text));
#endif
	emit newKeySignal(id, text);
}

void Call::enableDecrypt(char *passwd, int len)
{
#ifdef REN_DEBUG
	qWarning(QString("Call::enableDecrypt(char %1, int %2)").arg(passwd).arg(len));
#endif
	receiver->enableDecrypt(passwd, len);
}

void Call::enableRandomCrypt(int len)
{
#ifdef REN_DEBUG
	qWarning(QString("Call::enableRandomCrypt(int %1)").arg(len));
#endif
	char key[len];
	for (int i=0; i<len; i++)
		key[i] = (char)((rand()%256)-128);
	enableCrypt(key, len);
}

void Call::enableCrypt(char *passwd, int len)
{
#ifdef REN_DEBUG
	qWarning(QString("Call::enableCrypt(char *passwd, int %1)").arg(len));
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
#ifdef REN_DEBUG
	qWarning(QString("Call::send(char *data, int %1)").arg(len));
#endif
	// renyang - 有記錄, 且不是靜音
	if (recording && !muteRec)
		transmitter->sendAudioPacket(data, len);
}

void Call::message(QString text)
{
#ifdef REN_DEBUG
	qWarning(QString("Call::message(QString %1)").arg(text));
#endif
	warning(text);
}

void Call::warning(QString text)
{
#ifdef REN_DEBUG
	qWarning(QString("Call::warning(QString %1)").arg(text));
#endif
	emit warningSignal(id, text);
}

void Call::playRing()
{
#ifdef REN_DEBUG
	qWarning("Call::playRing()");
#endif
	transmitter->sendRingReplyPacket();
	if (!mutePlay)
		emit ringSignal(id);
}

void Call::playInit()
{
#ifdef REN_DEBUG
	qWarning("Call::playInit()");
#endif
	if (!mutePlay)
		emit initSignal(id);
}

void Call::ringMessage()
{
#ifdef REN_DEBUG
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
#ifdef REN_DEBUG
	qWarning(QString("Call::sendRing(bool %1)").arg(on));
#endif
	transmitter->ring(on);
}

// renyang - 替音訊解碼
void Call::decodeAudioData(char *buf, int len)
{
#ifdef REN_DEBUG
	qWarning("Call::decodeAudioData()");
#endif
	if (dec_state)
	{
		// renyang - Initializes the bit-stream from the data in an area of memory
		// renyang - 把一連串的buf轉到bit-stream
		speex_bits_read_from(&bits, buf, len);
		// renyang - 把bits解碼到outBuffer(為float型態)
		// renyang - 當<0表示解碼失敗
		if (speex_decode(dec_state, &bits, outBuffer) < 0)
		{
			emit warning("Warning: wrong decryption key or stream corrupted!");
			disableDecrypt();
		}
		else
		{
			// renyang - 來到這裡表示解碼成功
			// renyang - 把解碼之後的音訊(outBuffer)放到soundBuffer中
			putData(outBuffer, frame_size);
		}
	}
}

// renyang - 播放響鈴或是對方的語音
bool Call::playData(float *buf, int len)
{
#ifdef REN_DEBUG
	// qWarning(QString("Call::playData() - readyFrames: %1 len %2").arg(readyFrames).arg(len));;
	qWarning(QString("Call::playData(float %1, int %2)").arg(*buf).arg(len));
#endif
	bool ret = false;
	// renyang - 只有當真的有資料可以撥放時, 回傳才會是true
	if (readyFrames >= len)
	{
		if (!mutePlay)
		{
			for (int i=0; i<len; i++)
			{
				// renyang - 撥放是由Phone去撥放的
				buf[i] = soundBuffer[i];
			}
			ret = true;
		}
		// renyang - 由readyBuffer扣掉len長度
		updateFrames(len);
	}
	return ret;
}

bool Call::mixData(float *buf, int len, float balance)
{
#ifdef REN_DEBUG
	qWarning(QString("Call::mixData() - Balance: %1").arg(balance, 2, 'f', 1 ));
#endif
	bool ret = false;
	// renyang - 目前準備好的音訊檔的frames個數大於len
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

// renyang - 把解碼之後的音訊放到soundBuffer中
void Call::putData(float *buf, int len)
{
#ifdef REN_DEBUG
	qWarning(QString("Call::putData() - readyFrames: %1 len: %2").arg(readyFrames).arg(len));;
#endif
	// renyang - 若目前這一個stream的長度小於MAXBUFSIZE扣掉以經好的, 但是還沒有被讀取的stream還有空間的話
	// renyang - 則再塞資料進去
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
#ifdef REN_DEBUG
	qWarning("Call::disableCrypt()");
#endif
	transmitter->disableCrypt();
}

void Call::disableDecrypt()
{
#ifdef REN_DEBUG
	qWarning("Call::disableDecrypt()");
#endif
	receiver->disableDecrypt();
}

void Call::startRecorder()
{
#ifdef REN_DEBUG
	qWarning("Call::startRecorder()");
#endif
	recording = true;
}

void Call::muteRecorder(bool on)
{
#ifdef REN_DEBUG
	qWarning(QString("Call::muteRecorder(bool %1)").arg(on));
#endif
	muteRec = on;
}

void Call::mutePlayer(bool on)
{
#ifdef REN_DEBUG
	qWarning(QString("Call::mutePlayer(bool %1)").arg(on));
#endif
	mutePlay = on;
}

void Call::dumpRXStream(QString filename)
{
#ifdef REN_DEBUG
	qWarning(QString("Call::dumpRXStream(QString %1)").arg(filename));
#endif
	receiver->dump(filename);
}

void Call::dumpTXStream(QString filename)
{
#ifdef REN_DEBUG
	qWarning(QString("Call::dumpTXStream(QString %1)").arg(filename));
#endif
	transmitter->dump(filename);
}

bool Call::isDumpingRX()
{
#ifdef REN_DEBUG
	qWarning("Call::isDumpingRX()");
#endif
	return receiver->isDumping();
}

bool Call::isDumpingTX()
{
#ifdef REN_DEBUG
	qWarning("Call::isDumpingTX()");
#endif
	return transmitter->isDumping();
}

bool Call::isRXActive()
{
#ifdef REN_DEBUG
	qWarning("Call::isRXActive()");
#endif
	return receiver->isActive();
}

bool Call::isTXActive()
{
#ifdef REN_DEBUG
	qWarning("Call::isTXActive()");
#endif
	return transmitter->isActive();
}

bool Call::isFree()
{
#ifdef REN_DEBUG
	qWarning("Call::isFree()");
#endif
	return callFree;
}

QString Call::getCallerName()
{
#ifdef REN_DEBUG
	qWarning("Call::getCallerName()");
#endif
	return receiver->getCallerName();
}

// renyang - 取得對方的ip address
QString Call::getCallerIp()
{
#ifdef REN_DEBUG
	qWarning("Call::getCallerIp()");
#endif
	return receiver->getIp();
}

long Call::getTraffic()
{
#ifdef REN_DEBUG
	qWarning("Call::getTraffic()");
#endif
	// renyang - 用來記算傳送接收的總資料量
	long total = transmitter->getBytes() + receiver->getBytes();
	return total;
}

long Call::getTotalRX()
{
#ifdef REN_DEBUG
	qWarning("Call::getTotalRX()");
#endif
	return receiver->getTotal();
}

long Call::getTotalTX()
{
#ifdef REN_DEBUG
	qWarning("Call::getTotalTX()");
#endif
	return transmitter->getTotal();
}

long Call::getTotal()
{
#ifdef REN_DEBUG
	qWarning("Call::getTotal()");
#endif
	long total = transmitter->getTotal() + receiver->getTotal();
	return total;
}

void Call::setMyName(QString myName)
{
#ifdef REN_DEBUG
	qWarning(QString("Call::setMyName(QString %1)").arg(myName));
#endif
	transmitter->setMyName(myName);
}

// renyang - 把soundBuffer後面的部分移到前面來
void Call::updateFrames(int frames)
{
#ifdef REN_DEBUG
	qWarning(QString("Call::updateFrames(int %1)").arg(frames));
#endif
	// renyang - 我要讀取frames個, 所以, readyFrames會減少frames個
	readyFrames -= frames;
	if (readyFrames < 0)
		readyFrames = 0;
	if (readyFrames > 0)
	{
		// renyang - 把後面的資料copy到前面來
		memcpy(soundBuffer, soundBuffer+frames, sizeof(float)*readyFrames);
	}
}

bool Call::isRecording()
{
#ifdef REN_DEBUG
	qWarning("Call::isRecording()");
#endif
	return recording;
}

void Call::SlotgetIps(QStringList addrs_list)
{
#ifdef REN_DEBUG
	qWarning("Call::SlotgetIps()");
#endif
	emit SignalgetIps(id,addrs_list);
}

void Call::setPrimaddr(QString primaddr)
{
#ifdef REN_DEBUG
	qWarning(QString("Call::setPrimaddr(%1)").arg(primaddr));
#endif
	SctpSocketHandler::SctpSetPrim(sd,primaddr);
}
