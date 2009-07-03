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

// renyang-modify - 設定要求每一個frame若多久沒有收到則再要求下一個frame
#define VIDEO_WAIT_LIMIT 2000

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

	// renyang-modify 建立一個ip handler
	sctpiphandler = new SctpIPHandler();

	// renyang-modify - 固定時間內傳送data
	connect(sctpiphandler,SIGNAL(SigAddressConfrim(QString)),this,SLOT(SlotAddressConfirm(QString)));
	// renyang-modify - 當多次沒有收到資料時, 宣告這一個ip失聯
	connect(sctpiphandler,SIGNAL(SigAddressFail(QString)),this,SLOT(SlotAddressFail(QString)));
	// renyang-modify - 某一個ip有收到資料, 宣告這一個ip復活啦
	connect(sctpiphandler,SIGNAL(SigAddressAvailable(QString)),this,SLOT(SlotAddressAvailable(QString)));

	stopTimer = new QTimer(this);

	active = false;
	muteRec = false;
	mutePlay = false;
	callFree = true;
	aborted = false;
	recording = false;
	// renyang-modify - 目前是否有在要求對方的Video
	requestingVideo = false;

	// renyang-modify - 每要求一個frame後, 若固定時間內沒有收到完整的frame的話, 則再要求下一個frame
	videoTimer = new QTimer(this);
	connect(videoTimer,SIGNAL(timeout()),this,SLOT(sendVideoRequest()));

	// renyang-modify - 初始化傳送與接收image的index
	recvImage_index = sendImage_index = 0;

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
	// renyang-modify - 當Receiver接收到事件時, 會通知上層的Call, 以便修改CallTab的ip list情況
	connect (receiver,SIGNAL(SigAddressEvent(QString,QString)),this,SLOT(SlotAddressEvent(QString,QString)));
	// renyang-modify - 對方要求影像
	connect (receiver,SIGNAL(requestImage()),this,SLOT(SlotGetImage()));
	// renyang-modify - 由receiver接收到image資料, 並處理這一些資料
	connect (receiver,SIGNAL(newVideoData(char *,int)),this,SLOT(decodeVideoData(char *,int)));
	// renyang-modify - 當收到想要接收目前封包的下一部分時...
	connect (receiver,SIGNAL(requestNextImage()),this,SLOT(sendVideo()));
	// renyang-modify - 接收到完整的image，準備把它放到video_label
	connect (receiver,SIGNAL(completeImage()),this,SLOT(processImage()));
	// renyang-modify - 跟對方要求影像失敗
	connect (receiver,SIGNAL(requestImageFail()),this,SLOT(SlotrequestImageFail()));
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
	// renyang-modify - 刪掉sctpiphandler
	delete sctpiphandler;
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
		// renyang-modify - 設定目前沒有在要求對方的Video
		requestPeerImageStop();
		// renyang - 隔一段時間之後, 才正真close掉
		stopTimer->start(STOP_TIME, true);
		transmitter->end();
		receiver->end();
		// renyang-modify - 把所有的iphander內的資料全部清掉
		sctpiphandler->end();
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
		// renyang-modify - 設定stream no
		// transmitter->setStreamNo(0);
		// renyang - 接收端開始接收由對方回傳的資料並且判斷對方的動作
		// renyang - 開始查看是否有新的資料由peer端傳送過來
		receiver->start(sd, prot);
		// renyang-modify - 開始計時啦
		sctpiphandler->start();
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
		// renyang-modify - 開始計時啦
		sctpiphandler->start();
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
		// renyang-modify - 設定transmitter的stream no
		// transmitter->setStreamNo(0);
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
	qWarning(QString("Call::playData() - readyFrames: %1 len %2").arg(readyFrames).arg(len));;
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
	qWarning(QString("Call::putData() - readyFrames: %1 len: %2 remain_space:%3").arg(readyFrames).arg(len).arg(MAXBUFSIZE - readyFrames));;
#endif
	// renyang - 若目前這一個stream的長度小於MAXBUFSIZE扣掉以經好的, 但是還沒有被讀取的stream還有空間的話
	// renyang - 則再塞資料進去
	if (len < (MAXBUFSIZE - readyFrames))
	{
		memcpy(soundBuffer+readyFrames, buf, len*sizeof(float));
		// renyang - 準備好的資料長度增加len
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
#ifdef FANG_DEBUG
	qWarning("Call::SlotgetIps()");
#endif
	for (QStringList::Iterator it = addrs_list.begin();it != addrs_list.end();++it)
	{
		sctpiphandler->addIP(*it);
	}
	emit SignalgetIps(id,addrs_list);
}

// renyang-modify - 因接收到封包不是primary path而觸動更改primary path
void Call::setPrimaddr(QString primaddr)
{
#ifdef FANG_DEBUG
	qWarning(QString("Call::setPrimaddr(%1)").arg(primaddr));
#endif
	SctpSocketHandler::SctpSetPrim(sd,primaddr);
	// renyang-modify - 設定預期是由哪一個ip送資料過來
	receiver->setExpectAddress(primaddr);

	// renyang-modify - 更新stream number
	transmitter->updateStreamNo();

	// renyang-modify - 傳送一個封包告知對方目前要更新primary address
	// renyang-modify - 而這一個封包是用新的path去送的, 好讓對方知道是由哪一個address傳送過來的
	transmitter->sendPrimaryChangePacket();

	// renyang-modify - 設定CallTab的list圖示
	emit SigAddressEvent(id,primaddr,"PRIMARY ADDRESS");
	// renyang-modify - 表示剛剛改變完primary address

	if (requestingVideo)
	{
		// renyang-modify - 若在接收的情況下, 則在要求下一個Video
		sendVideoRequest();
	}
}

void Call::SlotAddressEvent(QString ip,QString description)
{
#ifdef FANG_DEBUG
	qWarning(QString("Call::SlotAddressEvent(%1,%2)").arg(ip).arg(description));
#endif
	// renyang-modify - 更新由這一個ip最近接收到資料的時間
	if (description == "RECVING")
	{
		sctpiphandler->setRecvingTime(ip);
	}
	if (description == "ADDRESS UNREACHABLE")
	{
		// renyang-modify - 不需要再探測，交由heartbeat去處理
		sctpiphandler->stopConfirmTimer(ip);
	}
	emit SigAddressEvent(id,ip,description);
}

// renyang-modify - 向Phone要求影像
void Call::SlotGetImage()
{
#ifdef FANG_DEBUG
	qWarning("Call::SlotGetImage()");
#endif
	// renyang-modify - 初始化目前送到這一個image_matrix的哪一個位置
	sendImage_index = 0;
	// renyang-modify - 要求Phone把video的資料放到這一個結構中
	emit SigGetImage((char *) &SendImage,id);
}

void Call::sendVideoFail()
{
#ifdef FANG_DEBUG
	qWarning("Call::sendVideoFail()");
#endif
	transmitter->sendVideoFailPacket();
}

// renyang-modify - 向對方傳送要求video的影像
void Call::sendVideoRequest()
{
#ifdef FANG_DEBUG
	qWarning("Call::sendVideoRequest()");
#endif
	transmitter->sendVideoRequestPacket();
	// renyang-modify - 設定目前的狀態是跟別人要求影像
	requestingVideo = true;
	// renyang-modify - 在接收前先把recvImage_index歸0
	recvImage_index = 0;
	// renyang-modify - 每當要求一個frame, 若固定時間內沒有收到一整個frame的話, 則要再重新要求一整個frame
	if (!videoTimer->isActive())
		videoTimer->start(VIDEO_WAIT_LIMIT,true);
}

// renyang-modify - 當收到IHU_INFO_VIDEO_NEXT的封包時, 接收端執行此函式, 第一次傳封包也是用此函式
void Call::sendVideo()
{
#ifdef FANG_DEBUG
	qWarning("Call::sendVideo()");
#endif
	// renyang-modify - sendImage_index是在SlotGetImage()初始化的
	if (sendImage_index == 0)
	{
		// renyang-modify - 取得要傳送的整個大小, 後面的那一個部分是記錄整個資料的長與寬
		// renyang-modify - 因為每一個pixel均由3 bytes代表, 所以整個image的大小要乘三
		send_left = 3*SendImage.width*SendImage.height + 2*sizeof(int);
	}

	if (send_left ==0)
	{
		// renyang-modify - 當沒有資料可以傳送時, 則送出結束的封包, 對方收到後, 會把抓到的資料(image)顯示在圖片上
		// renyang-modify - 這裡不需要初始化sendImage_index是因為
		// renyang-modify - 只有當對方要求一整個image的第一個封包時, 才會初始化sendImage_index
		transmitter->sendVideoEndPacket();
		// renyang-modify - 用來表示我已傳送一個EndPacket()給對方啦
		send_left = -1;
	}
	else if (send_left == -1)
	{
		// do nothing
	}
	else
	{
		// renyang-modify - 記錄這一個結構的起始位址
		char *ptr = (char *)&SendImage;
		// renyang-modify - 當傳送的資料比MAX_DATA_SIZE還大時, 則傳送MAX_DATA_SIZE的資料
		if (send_left >= MAX_DATA_SIZE)
		{
			transmitter->sendVideoPacket(ptr+sendImage_index,MAX_DATA_SIZE);
			sendImage_index += MAX_DATA_SIZE;
			send_left-=MAX_DATA_SIZE;
		}
		else
		{
			transmitter->sendVideoPacket(ptr+sendImage_index,send_left);
			sendImage_index += MAX_DATA_SIZE;
			send_left=0;
		}
	}
}

// renyang-modify - 處理視訊資料
void Call::decodeVideoData(char *buf, int len)
{
#ifdef FANG_DEBUG
	qWarning("Call::decodeVideoData()");
#endif
	char *ptr = (char *) &RecvImage;
	memcpy(ptr+recvImage_index,buf,len);
	recvImage_index+=len;
	// renyang-modify - 要求傳送下一個image的封包
	transmitter->sendVideoNextPacket();
}

// renyang-modify - 完整接收到一整個image啦
void Call::processImage()
{
#ifdef FANG_DEBUG
	qWarning("Call::processImage()");
#endif
	// renyang-modify - 收到一整個frame啦, 把videoTimer停止計時
	if (videoTimer->isActive())
		videoTimer->stop();

	// renyang-modify - 收到的資料必需剛好是120*160*3+8
	if (recvImage_index == 57608)
	{
		if (image.isNull() || (image.width()!=RecvImage.width) || (image.height() != RecvImage.height))
		{
			QImage temp(RecvImage.width,RecvImage.height,32);
			image = temp;
		}
		for (int y=0;y<RecvImage.height;y++) {
			for (int x=0;x<RecvImage.width;x++)
			{
				image.setPixel(x,y,qRgb(RecvImage.data[3*(x+y*RecvImage.width)+2],RecvImage.data[3*(x+y*RecvImage.width)+1],RecvImage.data[3*(x+y*RecvImage.width)]));
			}
		}
		emit SigputImage(image,id);
		// renyang-modify - 若沒有傳送影像上去, 就不會要求下一個影像啦
	}
	else
	{
		// renyang-modify - 這一個影像傳送失敗, 再要求下一個影像
		sendVideoRequest();
	}
}

// renyang-modify - 跟對方要求影像失敗
void Call::SlotrequestImageFail()
{
#ifdef FANG_DEBUG
	qWarning("Call::SlotrequestImageFail()");
#endif
	// renyang-modify - 不再等待對方傳送影像資料過來
	requestingVideo = false;
	emit SigrequestImageFail(id);
}

void Call::SlotAddressConfirm(QString address)
{
#ifdef FANG_DEBUG
	qWarning(QString("Call::SlotAddressNoRecv(%1)").arg(address));
#endif
	// renyang-modify - 只有對不是primary address的ip要傳送confirm packet
	if (address != receiver->getPrimAddress())
	{
		transmitter->lock_mutex();
		QString old_address = receiver->getPrimAddress();
		SctpSocketHandler::SctpSetPrim(sd,address);
		transmitter->sendConfirmPacket();
		SctpSocketHandler::SctpSetPrim(sd,old_address);
		transmitter->unlock_mutex();
	}
}

// renyang-modify - 某一個ip是不能通連的, 在固定時間內這一個ip都沒有收到任何資料
void Call::SlotAddressFail(QString address)
{
#ifdef FANG_DEBUG
	qWarning(QString("Call::SlotAddressFail(%1)").arg(address));
#endif
	// renyang-modify - 若這一個fail的ip剛好是目前的primary address的話, 則要更新primary address
	if (address == receiver->getPrimAddress())
	{
		// renyang-modify - 只有檢查primary address才要檢查是否有收到資料
		emit SigAddressEvent(id,address,QString("SlotAddressFail"));

		// renyang-modify - 設定某一個ip是與對方是不連通的
		sctpiphandler->setIPConnectable(address,false);

		QString availableIP = sctpiphandler->getAvailableIP();

		if (availableIP == "NO_AVAILABLE_IP")
		{
			qWarning("there is no available IP");
			// renyang-modify - TODO - 這裡要做某一些處理, 像是掛掉電話啦...
		}
		else
		{
			// renyang-modify - 設定新的primary address
			setPrimaddr(availableIP);
		}
	}
}

// renyang-modify - 表示某一個ip突然可以使用了
void Call::SlotAddressAvailable(QString address)
{
#ifdef FANG_DEBUG
	qWarning(QString("Call::SlotAddressAvailable(%1)").arg(address));
#endif
	emit SigAddressEvent(id,address,QString("SlotAddressAvailable"));
}

void Call::requestPeerImageStop()
{
#ifdef FANG_DEBUG
	qWarning("Call::requestPeerImageStop()");
#endif
	requestingVideo = false;
	if (videoTimer->isActive())
		videoTimer->stop();
}

void Call::AddressInfo(QString IP, bool enabled)
{
#ifdef REN_DEBUG
	qWarning(QString("Call::AddressInfo(%1,%2)").arg(IP).arg(enabled));
#endif
	sctpiphandler->setIPConnectable(IP,enabled);
}
