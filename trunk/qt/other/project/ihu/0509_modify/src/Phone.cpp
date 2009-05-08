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
#include "Phone.hpp"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define MAXBUFSIZE 65536
#define TICKTIME 18

#define VOL_FACTOR 4.0

#define IHU_RING_COUNT 75
#define IHU_INIT_COUNT 2

// renyang -  建立一個Phone最大可以同時撥出去的電話數(Max Call)
Phone::Phone(int mc)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::Phone(int %1)").arg(mc));
#endif
	maxcall = mc;
	// renyang - 建立一個陣列是用來儲放Call *
	calls = new Call*[maxcall];
	for (int i=0; i<maxcall; i++)
	{
		// renyang - 初始化使陣列每一個元件均存放NULL
		calls[i] = NULL;
	}
	call_number = 0;
	connections = 0;

	recorder = new Recorder();
	player = new Player();

	timer = new QTimer(this);

	recBuffer = NULL;
	ready = 0;

	enc_state = NULL;

/*
 * FIXME
 * The speex_mode_query() can give ony the frame size, but no sampling rate.
 * So we must create a useless decoder here, fetch the informations and then destroy it.
 * To fix this as soon as speex will add the speex_mode_query for sampling rate.
*/
	void *dec_state = speex_decoder_init(&speex_uwb_mode);
	int enh = 1;
	speex_decoder_ctl(dec_state, SPEEX_SET_ENH, &enh);
	speex_decoder_ctl(dec_state, SPEEX_GET_SAMPLING_RATE, &play_rate);
	speex_decoder_ctl(dec_state, SPEEX_GET_FRAME_SIZE, &play_frame_size);
	speex_decoder_destroy(dec_state);

	agc = false;
	agc_sw = false;
	agc_hw = false;
	peak = 0.f;
	amp = 1.f;
	agc_step = 0.f;
	agc_level = 0.f;
	agc_err = 0.f;

	inport = IHU_DEFAULT_INPORT;

	recording = false;
	playing = false;
	listening = false;
	ringing = false;

	notifier = NULL;
	tcpserver = NULL;
	sd = -1;

	rec_status = RECORDER_STATUS_STOP;
	play_status = PLAYER_STATUS_STOP;

	if ((out = (char *) malloc(MAXBUFSIZE))==NULL)
		throw Error(Error::IHU_ERR_MEMORY);
	if ((buffer = (float *) malloc(MAXBUFSIZE*sizeof(float)))==NULL)
		throw Error(Error::IHU_ERR_MEMORY);
	if ((prebuffer = (float *) malloc(REC_BUFSIZE*sizeof(float)))==NULL)
		throw Error(Error::IHU_ERR_MEMORY);
	if ((playBuffer = (float *) malloc(play_frame_size*2*sizeof(float)))==NULL)
		throw Error(Error::IHU_ERR_MEMORY);

	connect( recorder, SIGNAL(warning(QString)), this, SLOT(warning(QString)) );
	connect( recorder, SIGNAL(data(float*, int)), this, SLOT(processAudioSamples(float*,int)) );
	connect( player, SIGNAL(warning(QString)), this, SLOT(warning(QString)) );

	// renyang - 重點, 每一段時間到會檢查所有的Call
	connect( timer, SIGNAL(timeout()), this, SLOT(playCallback()) );
}

Phone::~Phone(void)
{
#ifdef REN_DEBUG
	qWarning("Phone::~Phone()");
#endif
	timer->stop();
	for (int i=0; i<maxcall; i++)
	{
		if (calls[i])
			deleteCall(i);
	}
	if (out)
		free(out);
	if (buffer)
		free(buffer);
	if (prebuffer)
		free(prebuffer);
	if (playBuffer)
		free(playBuffer);
	if (enc_state)
		speex_encoder_destroy(enc_state);
	delete[] calls;
	delete recorder;
	delete player;
}

void Phone::resize(int mc)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::resize(int %1)").arg(mc));
#endif
	if (mc != maxcall)
	{
		Call** oldcalls = calls;
		calls = new Call*[mc];
		for (int i=0; i<mc; i++)
			calls[i] = NULL;
		for (int i=0; i<maxcall; i++)
			calls[i] = oldcalls[i];
		maxcall = mc;
		delete[] oldcalls;
	}
}

// renyang-TODO - 要新增一個sctp
// renyang - 建立一個socket等待某人來連接
void Phone::waitCalls(int port, bool udp, bool tcp)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::waitCalls(port:%1,udp:%2,tcp:%3)").arg(port).arg(udp).arg(tcp));
#endif
	inport = port;

	// renyang - 在同一台電腦同一張網卡udp, tcp可以同時bind相同的port
	if (udp)
	{
		if ((sd = ::socket(AF_INET, SOCK_DGRAM, 0)) == -1)
			throw Error(tr("Can't initalize socket! (socket())"));
		if (::fcntl(sd, F_SETFL, O_NONBLOCK) == -1)
			throw Error(tr("fcntl can't set socket property\n") + strerror(errno));
	
		memset(&sa, 0, sizeof(sa));
		sa.sin_family = AF_INET;
		sa.sin_port = htons(port);
		sa.sin_addr.s_addr = htonl(INADDR_ANY);
	
		int opt = 1;
		// renyang - 使得可以多個應用程式使用同一個port號
		if (::setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt) )==-1)
			throw Error(tr("setsockopt error"));

		if (::bind(sd, (struct sockaddr *) &sa, sizeof(sa)) == -1)
			throw Error(tr(QString("can't listen on UDP port %1 (%2)").arg(port).arg(strerror(errno))));
		
		notifier = new QSocketNotifier(sd, QSocketNotifier::Read, this);
		connect(notifier,SIGNAL(activated(int)),this, SLOT(newUDPConnection(int)));
	}
	if (tcp)
	{
		// renyang - 建立一個tcp server的物件, 等待別人來連線
		tcpserver = new TcpServer(this, port);
		// renyang - 當server端有新的連線進來時, newConnect函式會被emit[這是系統內定的]
		// renyang -  newConnect(int)其中的int參數表示client socket file descriptor
		connect( tcpserver, SIGNAL(newConnect(int)), this, SLOT(newTCPConnection(int)));
	}

	// renyang - modify - start
	int sctp = true;
	if (sctp)
	{
		if ((sctp_sd = ::socket(AF_INET,SOCK_SEQPACKET,IPPROTO_SCTP)) == -1)
			throw Error(tr("Can't initalize sctp socket! (socket())"));
		if (::fcntl(sctp_sd,F_SETFL,O_NONBLOCK) == -1)
			throw Error(tr("fcntl can't set socket property\n") + strerror(errno));

		sctp_notifier = new QSocketNotifier(sctp_sd,QSocketNotifier::Read,this);
		connect(sctp_notifier,SIGNAL(activated(int)),this,SLOT(newSCTPConnection(int)));

		memset(&sctp_sa,0,sizeof(sctp_sa));
		sctp_sa.sin_family = AF_INET;
		sctp_sa.sin_port = htons(port);
		sctp_sa.sin_addr.s_addr = htonl(INADDR_ANY);
		
		// renyang - 讓sctp可以關掉之後, 短時間再啟動
		int opt=1;
		if (::setsockopt(sctp_sd,SOL_SOCKET,SO_REUSEADDR,(char *)&opt,sizeof(opt))==-1)
			throw Error(tr("setsockopt error"));
		
		if (::bind(sctp_sd,(struct sockaddr *) &sctp_sa,sizeof(sctp_sa)) == -1)
			throw Error(tr(QString("can't bind on SCTP port %1 (%2)").arg(port).arg(strerror(errno))));

		struct sctp_event_subscribe events;
		memset((void *)&events,0,sizeof(events));
		events.sctp_data_io_event = 1;
		if (::setsockopt(sctp_sd,SOL_SCTP,SCTP_EVENTS,(const void *)&events,sizeof(events))==-1)
			throw Error(tr(QString("can't set io event")));

		if (::listen(sctp_sd,5)==-1)
			throw Error(tr(QString("can't listen on SCTP port %1 (%2)").arg(port).arg(strerror(errno))));

		qWarning("Start SCTP server...");
	}
	// renyang - modify - end

	listening = true;
}

// renyang - 此socket代表client端的socket file descriptor
void Phone::newTCPConnection(int socket)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::newTCPConnection(socket:%1)").arg(socket));
#endif
	// renyang - 連線數增加
	connections++;
	// renyang - 取得一個新的Call ID
	int callId = newCall();
	if (callId >= 0)
	{
		calls[callId]->start(socket, IHU_TCP);
		receivedCall(callId);
	}
	else
	{
		::close(socket);
	}
}

// renyang - 建立一個UDP的連線
// renyang - 感覺應該是別人送一個udp封包過來，因此這裡就建立一個udp來服務它???
void Phone::newUDPConnection(int socket)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::newUDPConnection(%1)").arg(socket));
#endif
	// renyang - 網路連線數加1
	connections++;
	// renyang - 若沒有馬上刪除，會一直觸發目前這一個函式
	// renyang - 因為你還沒有把資料處理掉, 所以, 它會一直觸發
	// renyang - 因為刪掉了notifier, 所以, 在這個函式最下面還要重新連立一個udp的socket
	delete notifier;
	notifier = NULL;
	// renyang - 回傳一個空閒的Call Id(若沒有的話, 此函式會自己建立)
	int callId = newCall();
	if (callId >= 0)
	{
		// renyang - 把socket丟下去給它接受別人傳送過來的資料
		calls[callId]->start(socket, IHU_UDP);
		receivedCall(callId);
	}
	else
	{
		close(socket);
	}
	// renyang - 在server端再建立一個udp socket, 等待下一個通話, 否則當資料傳送進來, 會當作有新的電話進來
	// renyang - 一直會出現CallTab
	waitCalls(inport, true, false);
}

void Phone::newSCTPConnection(int socket)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::newSCTPConnection(%1)").arg(socket));
#endif
	connections++;
	delete sctp_notifier;
	sctp_notifier = NULL;
	int callId = newCall();
	if (callId >=0)
	{
		calls[callId]->start(socket,IHU_SCTP);
		receivedCall(callId);
	}
	else
	{
		close(socket);
	}
	qWarning("newSCTPConnection");
}

// renyang - 當有電話進來, 改變圖示, 開始播放器
void Phone::receivedCall(int callId)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::receivedCall(int %1)").arg(callId));
#endif
	try
	{
		call_number++;
		startPlayer();
		timer->start(TICKTIME, false);
		emit receivedCallSignal(callId);
	}
	catch (Error e)
	{
		emit abortSignal(e.getText());
	}
}

// renyang - 傳回一個目前沒有在使用的Call Id
int Phone::newCall()
{
#ifdef REN_DEBUG
	qWarning("Phone::newCall()");
#endif
	// renyang - 回傳一個call id目前沒有被使用(也就是新增的一個CallTab但目前沒有被使用)
	int callId = findFreeCall();
	// renyang - 尋找不到空的call id, 自己建立一個(也就是目前的所有CallTab均被使用中)
	if (callId == -1)
	{
		try
		{
			callId = createCall();
			emit newCallSignal(callId);
		}
		catch (Error e)
		{
// FIXME		Nothing to do?
		}
	}
//	qWarning(QString("Phone::newCall() = %1").arg(callId));
	return callId;
}

// renyang-TODO - 當停止接收call時, sctp要做什麼動作
void Phone::stopWaiting()
{
#ifdef REN_DEBUG
	qWarning("Phone::stopWaiting()");
#endif
	listening = false;

	if(tcpserver)
		delete tcpserver;
	tcpserver = NULL;

	if (notifier)
		delete notifier;
	notifier = NULL;

	if (sd != -1)
		close(sd);
	sd = -1;

	connections = 0;
}

// renyang - 建立一個新的Call
int Phone::createCall()
{
#ifdef REN_DEBUG
	qWarning("Phone::createCall()");
#endif
	int newId = findFreeId();
	if (newId < 0)
	{
		throw Error(QString("max number of calls reached (%1)").arg(maxcall));
	}
	else
	{
		// renyang - 為目前找到的這一個call id建立一個實體的Call
		calls[newId] = new Call(newId, myName);
		// renyang - 某一個call剛剛建立通話
		connect( calls[newId], SIGNAL(connectedSignal(int)), this, SLOT(connectedCall(int)) );	
		connect( calls[newId], SIGNAL(cancelCallSignal(int)), this, SLOT(cancelCall(int)) );	
		connect( calls[newId], SIGNAL(abortSignal(int, QString)), this, SLOT(abortCall(int, QString)) );
		connect( calls[newId], SIGNAL(warningSignal(int, QString)), this, SLOT(callWarning(int, QString)) );
		connect( calls[newId], SIGNAL(newKeySignal(int, QString)), this, SLOT(receivedNewKey(int, QString)) );
		connect( calls[newId], SIGNAL(cryptedSignal(int)), this, SLOT(cryptCall(int)) );	
		connect( calls[newId], SIGNAL(initSignal(int)), this, SLOT(playInit(int)) );	
		connect( calls[newId], SIGNAL(ringSignal(int)), this, SLOT(playRing(int)) );	
	}
	return newId;
}

void Phone::deleteCall(int callId)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::deleteCall(%1)").arg(callId));
#endif
	Call *old_call = calls[callId]; 
	calls[callId] = NULL;
	if (old_call)
		delete old_call; 
}

// renyang - 通知上層連線成功, 接受對方打來的電話, 或是本地端接受對方打過來的電話
void Phone::connectedCall(int callId)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::connectedCall(%1)").arg(callId));
#endif
	emit connectedCallSignal(callId);
}

void Phone::abortCall(int callId, QString text)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::abortCall(int %1, QString %2)").arg(callId).arg(text));
//	qWarning("Phone::abortCall(int, QString)");
#endif
	abortCall(callId);
	emit abortCallSignal(callId, text);
}

void Phone::abortCall(int callId)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::abortCall(int %1)").arg(callId));
#endif
//	qWarning("Phone::abortCall(int)");
	if (calls[callId])
		calls[callId]->error();
}

void Phone::callWarning(int callId, QString text)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::callWarning(int %1, QString %2)").arg(callId).arg(text));
#endif
	emit messageSignal(callId, text);
}

void Phone::receivedNewKey(int callId, QString text)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::receivedNewKey(int %1, QString %2)").arg(callId).arg(text));
#endif
	emit newKeySignal(callId, text);
}

void Phone::stopCall(int callId)
{
#ifdef REN_DEBUG
	qWarning("Phone::stopCall()");
#endif
	if (calls[callId])
		calls[callId]->stop();
}

void Phone::cancelCall(int callId)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::cancelCall(%1)").arg(callId));
#endif
	checkSound();
	emit cancelCallSignal(callId);
}

void Phone::endCall(int callId)
{
#ifdef REN_DEBUG
	qWarning("Phone::endCall()");
#endif
	if (calls[callId])
		calls[callId]->end();
}

void Phone::endAll()
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::endAll()"));
#endif
	for (int i=0; i<maxcall; i++)
	{
		if (calls[i])
			calls[i]->end();
	}
}

void Phone::abortAll()
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::abortAll()"));
#endif
	for (int i=0; i<maxcall; i++)
	{
		if (calls[i])
		{
			calls[i]->error();
			calls[i]->stop();
		}
	}
}

// renyang - 與host ip建立連線，port號, Protocol
void Phone::call(int callId, QString host, int port, int prot)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::call(%1, %2, %3, %4)").arg(callId).arg(host).arg(port).arg(prot));
	prot = IHU_SCTP;
#endif
	// renyang - 暫時 - start
	prot = IHU_SCTP;
	// renyang - 暫時 - end
	if (calls[callId])
	{
		try
		{
			calls[callId]->call(host, port, prot);
			startPlayer();
			startRecorder();
			timer->start(TICKTIME, false);
			call_number++;
		}
		catch (Error e)
		{
			abortCall(callId);
			e.setCallId(callId);
			throw e;
		}
	}
	else throw Error(QString("invalid call ID"));
}

// renyang - 此id的call接受別人打過來的電話
void Phone::answerCall(int callId)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::answerCall(%1) - connections=%2").arg(callId).arg(connections));
#endif
	if (calls[callId])
	{
		try
		{
			startRecorder();
			calls[callId]->answer();
			if (connections > 0)
				connections--;
		}
		catch (Error e)
		{
			abortCall(callId);
			e.setCallId(callId);
			throw e;
		}
	}
}

void Phone::setupAdr(bool on, float min, float max, float stretch)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::setupAdr(bool %1, float %2, float %3, float %4)").arg(on).arg(min).arg(max).arg(stretch));
#endif
	player->setAdr(on, min, max, stretch);
}

void Phone::setupAgc(bool on, bool hw_on, bool sw_on, float step, float lev, const char* mixname)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::setupAgc(bool %1, bool %2, bool %3, float %4, float %5, const %6)").arg(on).arg(hw_on).arg(sw_on).arg(step).arg(lev).arg(mixname));
#endif
	agc_hw = hw_on & on;
	agc_sw = sw_on & on;
	agc_step = step;
	if (lev <= -96.0)
		agc_level = 0.0;
	else agc_level = (float) pow(10.0, lev/20.0);
	amp = 1.f;
	agc_err = 0.f;
	try
	{
		recorder->setupAgc(agc_hw, mixname);
		agc = on;
	}
	catch (Error e)
	{
		throw e;
	}
}

void Phone::disableRecorder(bool on)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::disableRecorder(bool %1)").arg(on));
#endif
	if (on)
	{
		stopRecorder();
		rec_status = RECORDER_STATUS_MUTE;
	}
	else
	{
		rec_status = RECORDER_STATUS_STOP;
		if (call_number > 0)
		{
			for (int i=0; i<maxcall; i++)
			{
				if (calls[i])
				{
					if (calls[i]->isRecording())
					{
						startRecorder();
						break;
					}
				}
			}
		}
	}
}

void Phone::mutePlayer(int callId, bool on)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::mutePlayer(int %1, bool %2)").arg(callId).arg(on));
#endif
	if (calls[callId])
		calls[callId]->mutePlayer(on);
}

void Phone::muteRecorder(int callId, bool on)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::muteRecorder(int %1, bool %2)").arg(callId).arg(on));
#endif
	if (calls[callId])
		calls[callId]->muteRecorder(on);
}

void Phone::disablePlayer(bool on)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::disablePlayer(bool %1)").arg(on));
#endif
	if (on)
	{
		stopPlayer();
		play_status = PLAYER_STATUS_MUTE;
	}
	else
	{
		play_status = PLAYER_STATUS_STOP;
		if (call_number > 0)
		{
			startPlayer();
		}
	}
}

void Phone::enableRandomCrypt(int callId, int len)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::enableRandomCrypt(int %1, int %2)").arg(callId).arg(len));
#endif
	if (calls[callId])
		calls[callId]->enableRandomCrypt(len);
}

void Phone::enableCrypt(int callId, char *passwd, int len)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::enableCrypt(int %1, char *passwd, int %2)").arg(callId).arg(len));
#endif
	if (calls[callId])
		calls[callId]->enableCrypt(passwd, len);
}

void Phone::enableDecrypt(int callId, char *passwd, int len)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::enableDecrypt(int %1, char *passwd, int %2)").arg(callId).arg(len));
#endif
	if (calls[callId])
		calls[callId]->enableDecrypt(passwd, len);
}

void Phone::disableDecrypt(int callId)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::disableDecrypt(int callId)"));
#endif
	if (calls[callId])
		calls[callId]->disableDecrypt();
}

void Phone::disableCrypt(int callId)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::disableCrypt(int callId)"));
#endif
	if (calls[callId])
		calls[callId]->disableCrypt();
}

void Phone::setMyName(QString name)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::setMyName(QString name)"));
#endif
	if(name.length() > IHU_MAX_NAME_LEN)
	{
		// renyang - 會被截為IHU_MAX_NAME_LEN的長度
		// renyang - 事實上可以不用以上的判斷，因為若小於設定的最大長度的話，則什麼動作也不會發生
		name.truncate(IHU_MAX_NAME_LEN);
	}
	myName = name;
	for (int i=0; i<maxcall; i++)
	{
		if (calls[i])
			calls[i]->setMyName(myName);
	}
}

// renyang - 設定語音的編碼設定
void Phone::setup(int mode, int quality, int abr, int vbr, float vbr_quality, int complexity, int vad, int dtx, int txstop, int th, int ring_vol)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::setup"));
#endif
	bool restart = recording;
	if (restart)
		stopRecorder();

	if (enc_state)
		speex_encoder_destroy(enc_state);

	SpeexMode *spmode = NULL;
	
	// renyang - 依目前的頻寬來設定語音品質
	switch (mode)
	{
		case IHU_NARROW:
			spmode = (SpeexMode *) &speex_nb_mode;
			break;
		case IHU_WIDE:
			spmode = (SpeexMode *) &speex_wb_mode;
			break;
		case IHU_ULTRAWIDE:
			spmode = (SpeexMode *) &speex_uwb_mode;
			break;
	}
	
	// renyang - 初始化一個編碼器
	enc_state = speex_encoder_init(spmode);

	// renyang - 類似ioctl, 直接對底層的部分操作
	// renyang - 設定encode的複雜度
	speex_encoder_ctl(enc_state, SPEEX_SET_COMPLEXITY, &complexity);
	// renyang - 使用的編碼是vbr格式
	if (vbr)
	{
		// renyang - 設定vbr是否要打開
		speex_encoder_ctl(enc_state, SPEEX_SET_VBR, &vbr);
		// renyang - 設定vbr的quality
		speex_encoder_ctl(enc_state, SPEEX_SET_VBR_QUALITY, &vbr_quality);
	}
	else
	{
		// renyang - 設定一般語音的quality
		speex_encoder_ctl(enc_state, SPEEX_SET_QUALITY, &quality);
		// renyang - 設定VAD (Voice activity detection)音聲變動偵測
		speex_encoder_ctl(enc_state, SPEEX_SET_VAD, &vad);
	}
	if (abr)
	{
		// renyang - 設定abr是否要開啟
		speex_encoder_ctl(enc_state, SPEEX_SET_ABR, &abr);
	}
	// renyang - 設定DTX的狀態
	speex_encoder_ctl(enc_state, SPEEX_SET_DTX, &dtx);

	// renyang - Get sampling rate used in bit-rate computation
	speex_encoder_ctl(enc_state, SPEEX_GET_SAMPLING_RATE, &rate);
	// renyang - 取得一個frame的大小
	speex_encoder_ctl(enc_state, SPEEX_GET_FRAME_SIZE, &frame_size);

	stoptx = txstop;
	
	speex_bits_init(&enc_bits);
	
	ring_vol = -ring_vol + 1;
	float vol = 0.0;
	if (ring_vol <= 0)
		vol = powf(VOL_FACTOR, (float) (ring_vol));
	for (int i=0; i<SIZE_RING_32; i++)
		ringBuffer[i] = ((float) ring_32[i]) * vol;

	setThreshold(th);

	if (restart)
		startRecorder();
}

void Phone::setThreshold(int th)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::setThreshold(int th)"));
#endif
	float input = (float) th;
	if (input <= -96.0)
		threshold = 0.0;
	else threshold = (float) pow(10.0, input/20.0);
	threshold *= 32768.0;
}

void Phone::setupRecorder(int driver, QString interface)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::setupRecorder(int driver, QString interface)"));
#endif
	// renyang - driver是指使用ALSA或是JACK當作音訊的編碼
	recorder->setup(driver, interface);
}

void Phone::setupPlayer(QString interface, int prepackets)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::setupPlayer"));
#endif
	player->setup(interface, prepackets);
}

void Phone::ring(int callId, bool on)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::ring(int callId, bool on)"));
#endif
//	qWarning("Phone::ring()");
	if (calls[callId])
		calls[callId]->sendRing(on);
}

void Phone::processAudioSamples(float *samples, int nsample)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::processAudioSamples"));
#endif
	recBuffer = samples;
	while (nsample > 0)
	{
		toRead = frame_size - ready;
		// renyang - 目前送過來的音訊封包還是小於必需要讀取的封包
		if (nsample < toRead)
		{
			memcpy(bufptr, recBuffer, nsample*sizeof(float));
			ready += nsample;
			bufptr += nsample;
			nsample = 0;
			break;
		}
		else if (nsample >= toRead)
		{
			// renyang - 會到這裡表示目前由麥克風收到的封包大於缺少的封包個數
			memcpy(bufptr, recBuffer, toRead*sizeof(float));
			ready += toRead;
			bufptr += toRead;
			recBuffer += toRead;
			nsample -= toRead;
			if (ready == frame_size)
			{
				sendAudioData(buffer, frame_size);
				ready = 0;
				bufptr = buffer;
			}
		}
	}
}

// renyang - 應該是開始擷取麥克風的資料
void Phone::startRecorder()
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::startRecorder()"));
#endif
//	qWarning("Phone::startRecorder()");
	switch (rec_status)
	{
		// renyang - 哇哈哈, 目前是靜音, 不處理
		case RECORDER_STATUS_MUTE:
			break;
		default:
			// renyang - 是否要記錄
			if (!recording)
			{
				// renyang - 先清空預先要準備資訊(音訊???)的空間
				memset(prebuffer, 0x0, sizeof(float)*REC_BUFSIZE);
				// renyang - 預先清楚要送資料出去的空間???
				// renyang - 所以資料應該是先送到預先準備的資料中, 最後再送到要送出去的資料中???
				memset(out, 0x0, MAXBUFSIZE);
				// renyang - 還沒有開始說話???
				speak = 0;
				// renyang - 還沒有準備好???
				// renyang - 一個是預先準備的buffer好了沒, 另一個是要送出去的buffer準備好了沒
				ready = toRead = 0;
				bufptr = buffer;
				// renyang - 開始讀取麥克風的聲音啦
				recorder->start(rate);
				// renyang - 設定目前的記錄器為等待
				rec_status = RECORDER_STATUS_WAITING;
				// renyang - 開始擷取麥克風的語音
				recording = true;
				// renyang - 告知別人(IhuGui), 我要開始記錄啦
				// renyang - 耶，這一個部分只有對Ihu有影響
				emit recorderSignal(true);
			}
			break;
	}
}

void Phone::stopRecorder()
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::stopRecorder()"));
#endif
	switch (rec_status)
	{
		case RECORDER_STATUS_MUTE:
			break;
		default:
			rec_status = RECORDER_STATUS_STOP;
			recorder->end();
			recording = false;
			emit recorderSignal(false);
			break;
	}
}

// renyang - 開始執行播放器
void Phone::startPlayer()
{
#ifdef REN_DEBUG
	qWarning("Phone::startPlayer()");
#endif
	switch (play_status)
	{
		// renyang - 若是靜音設定的話, 則直接跳出
		case PLAYER_STATUS_MUTE:
			break;
		default:
			// renyang - 若沒有設定播放的話, 再設定開始撥放
			if (!playing)
			{
				player->start(play_rate, play_frame_size);
				play_status = PLAYER_STATUS_RUNNING;
				playing = true;
				// renyang - 改變GUI的變化
				emit playerSignal(true);
			}
			break;
	}
}

void Phone::stopPlayer()
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::stopPlayer()"));
#endif
//	qWarning("Phone::stopPlayer()");
	switch (play_status)
	{
		case PLAYER_STATUS_MUTE:
			break;
		default:
			play_status = PLAYER_STATUS_STOP;
			player->end();
			playing = false;
			emit playerSignal(false);
			break;
	}
}

// renyang - 尋找出最大音量, 來確認是有在說話
bool Phone::isSpeaking(float *samples, int nsample)
{
#ifdef REN_DEBUG	
	qWarning(QString("Phone::isSpeaking"));
#endif
	bool ret = false;
	float smp;
	// renyang - 尋找最大音量的樣本數
	for (int i=0; i<nsample; i++)
	{
		// renyang - samples[i] = samples[i] * amp;
		samples[i] *= amp;
		smp = fabs(samples[i]);
		if (smp > peak)
			peak = smp;
	}
	if (peak >= threshold)
		ret = true;
	return ret;
}

// renyang - 傳送音訊資料
void Phone::sendAudioData(float *sample, int samples)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::sendAudioData"));
#endif
	// renyang - 若最大音量有超過threshold, 則表示要傳送語音資料
	if (isSpeaking(sample, samples))
	{
		if (rec_status == RECORDER_STATUS_WAITING)
			rec_status = RECORDER_STATUS_START;
		// renyang - 不是很懂這一個程式碼要做什麼???
		speak = stoptx; // To guarantee stream even for small speech pauses
	}
	switch (rec_status)
	{
		// renyang - 開始傳送音訊, 並且把目前的狀態改為正在傳送音訊
		case RECORDER_STATUS_START:
			send(prebuffer, frame_size);
			rec_status = RECORDER_STATUS_RUNNING;
			// renyang - 不知道這裡是不是少一個break???
		case RECORDER_STATUS_RUNNING:
			if (speak > 0) {
				send(sample, samples);
				break;
			}
			else 
			{
				// renyang - 跳到等待的狀態
				rec_status = RECORDER_STATUS_WAITING;
			}
		case RECORDER_STATUS_WAITING:  // It keeps the previous buffer to anticipate a little the speech
			memcpy(prebuffer, sample, samples*sizeof(float));
		case RECORDER_STATUS_MUTE:
		case RECORDER_STATUS_STOP:
			break;
	}
}

void Phone::send(float *fsamples, int samples)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::send"));
#endif
	int tot = 0;
	// renyang - Resets bits to initial value (just after initialization, erasing content)
	speex_bits_reset(&enc_bits);
	if (enc_state)
	{
		// renyang - int speex_encode(void *state, float *in, SpeexBits *bits);
		// renyang - 編碼
		speex_encode(enc_state, fsamples, &enc_bits);
		// renyang - int speex_bits_write(SpeexBits *bits, char *bytes, int max_len);
		// renyang - Write the content of a bit-stream to an area of memory
		// 把語音編碼寫入out
		tot = speex_bits_write(&enc_bits, out, MAXBUFSIZE);
	}
	if (tot > 0) {
		try
		{
			for (int i=0; i<maxcall; i++)
			{
				// renyang - 看目前哪一個call有在用, 就送出去
				if (calls[i])
				{
					// renyang - 把語音資料傳送出去
					// renyang - 把所有的calls的資料傳送出去
					calls[i]->send(out, tot);
				}
			}
		}
		catch (Error e)
		{
			abortCall(e.getCallId(), e.getText());
		}
	}
}

int Phone::findFreeId()
{
#ifdef REN_DEBUG
	qWarning("Phone::findFreeId()");
#endif
	// renyang - 當找過所有的id, 仍發現所有的id仍在使用, 則回傳-1
	for (int i=0; i<maxcall; i++)
	{
		if (calls[i] == NULL)
			return i;
	}
	return -1;
}

// renyang - 尋找哪一個id目前沒有被使用
int Phone::findFreeCall()
{
#ifdef REN_DEBUG
	qWarning("Phone::findFreeCall()");
#endif
	for (int i=0; i<maxcall; i++)
	{
		if (calls[i])
		{
			if (calls[i]->isFree())
				return i;
		}
	}
//	qWarning(QString("Phone::findFreeCall() = -1"));
	return -1;
}

void Phone::warning(QString text)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::warning(%1)").arg(text));
#endif
	emit warningSignal(text);
}

QString Phone::getCallerName(int callId)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::getCallerName(%1)").arg(callId));
#endif
	QString ret = "";
	if (calls[callId])
		ret = calls[callId]->getCallerName();
	return ret;
}

// renyang - 取得caller id的ip address
QString Phone::getCallerIp(int callId)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::getCallerIp(%1)").arg(callId));
#endif
	QString ret = "";
	if (calls[callId])
		ret = calls[callId]->getCallerIp();
	return ret;
}

long Phone::getCallTraffic(int callId)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::getCallTraffic(%1)").arg(callId));
#endif
	long ret = 0;
	if (calls[callId])
		ret = calls[callId]->getTraffic();
	return ret;
}

long Phone::getCallRX(int callId)
{
#ifdef REN_DEBUG
	qWarning(QString("getCallRX(int %1)").arg(callId));
#endif
	long ret = 0;
	if (calls[callId])
		ret = calls[callId]->getTotalRX();
	return ret;
}

long Phone::getCallTX(int callId)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::getCallTX(int %1)").arg(callId));
#endif
	long ret = 0;
	if (calls[callId])
		ret = calls[callId]->getTotalTX();
	return ret;
}

long Phone::getTotal()
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::getTotal()"));
#endif
	long total = 0;
	for (int i=0; i<maxcall; i++)
	{
		if (calls[i])
			total += calls[i]->getTotal();
	}
	return total;
}

void Phone::checkSound()
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::checkSound() - call_number = %1").arg(call_number));
#endif
	if (--call_number <= 0)
	{
		call_number = 0;
		stopRecorder();
		timer->stop();
		stopPlayer();
	}
}

void Phone::cryptCall(int callId)
{
#ifdef REN_DEBUG
	qWarning("Phone::cryptCall()");
#endif
	emit cryptedSignal(callId);
}

void Phone::dumpRXStream(int callId, QString filename)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::dumpRXStream(%1, %2)").arg(callId).arg(filename));
#endif
	if (calls[callId])
		calls[callId]->dumpRXStream(filename);
}

void Phone::dumpTXStream(int callId, QString filename)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::dumpTXStream(%1, %2)").arg(callId).arg(filename));
#endif
	if (calls[callId])
		calls[callId]->dumpTXStream(filename);
}

bool Phone::isDumpingRX(int callId)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::isDumpingRX(%1)").arg(callId));
#endif
	bool ret = false;
	if (calls[callId])
		ret = calls[callId]->isDumpingRX();
	return ret;
}

bool Phone::isDumpingTX(int callId)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::isDumpingTX(%1)").arg(callId));
#endif
	bool ret = false;
	if (calls[callId])
		ret = calls[callId]->isDumpingTX();
	return ret;
}

bool Phone::isRXActive(int callId)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::isRXActive(%1)").arg(callId));
#endif
	bool ret = false;
	if (calls[callId])
		ret = calls[callId]->isRXActive();
	return ret;
}

bool Phone::isTXActive(int callId)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::isTXActive(%1)").arg(callId));
#endif
	bool ret = false;
	if (calls[callId])
		ret = calls[callId]->isTXActive();
	return ret;
}

// renyang - 取得player的delay
float Phone::getDelay()
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::getDelay()"));
#endif
	return player->getDelay();
}

bool Phone::isListening()
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::isListening()"));
#endif
	return listening;
}

int Phone::getConnections()
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::getConnections()"));
#endif
	return connections;
}

int Phone::getCalls()
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::getCalls()"));
#endif
	return call_number;
}

void Phone::clearConnections()
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::clearConnections()"));
#endif
	connections = 0;
}

int Phone::getPeak()
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::getPeak()"));
#endif
	if (speak > 0)
		speak--;

	peak /= 32768.0;

	if (agc)
	{
		switch (rec_status)
		{
			case RECORDER_STATUS_RUNNING:
				agc_err = 0.5*agc_err + 0.5*(agc_level - peak);
				if (agc_hw)
					recorder->adjustVolume(agc_err*agc_step);
				if (agc_sw)
				{
					//fprintf(stderr, "AGC Software: Level=%f Peak=%f Error=%f Amp=%f\n", agc_level, peak, agc_err, amp);
					amp += agc_err*agc_step;
					if (amp < 0.f)
						amp = 0.f;
				}
				break;
			default:
				break;
		}
	}

	if (peak <= 0.0)
		peak = -96.0;
	else peak = 20 * log10(peak);
	int ret = (int) peak;
	// renyang - 設定為float的0
	peak = 0.f;
	return ret;
}

// renyang - 每18毫秒會執行一次, 把準備好的音訊撥放出來(由網路收到的音訊撥放出來)
void Phone::playCallback()
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::playCallback()"));
#endif
	// renyang - 設定是否有對方的聲音, 當silence=true則表示有聲音(可能是鈴聲或是對方的聲音)
	bool silence = true;
	balance = 2.f;
	// renyang - 設定響鈴次數
	if (ringing)
	{
		if (ringCount-- > 0)
		{
			player->disableADR(true);
			for (int i=0; i<play_frame_size; i++)
				playBuffer[i] = ringBuffer[ringPtr+i];
			ringPtr += play_frame_size;
			if (ringPtr >= SIZE_RING_32)
				ringPtr = 0;
			silence = false;
		}
		else
		{
			ringing = false;
			player->disableADR(false);
		}
	}
	for (int i=0; i<maxcall; i++)
	{
		if (calls[i])
		{
			if (silence)
			{
				// renyang - 把某一個call的ready的檔案copy到playBuffer中
				if (calls[i]->playData(playBuffer, play_frame_size))
				{
					silence = false;
				}
			}
			else
			{
				if (calls[i]->mixData(playBuffer, play_frame_size, 1.f/balance))
				{
					balance++;
				}
			}
		}
	}
	switch (play_status)
	{
		case PLAYER_STATUS_RUNNING:
			if (!silence)
			{
				// renyang - 把剛剛抓出來的資料, 放到player中
				player->put(playBuffer, play_frame_size);
			}
			// renyang - 實際放出音量啦
			player->callback();
			break;
		default:
			break;
	}
}

void Phone::playInit(int callId)
{
#ifdef REN_DEBUG
//	qWarning("Phone::playInit()");
	qWarning(QString("Phone::playInit(int %1)").arg(callId));
#endif
	ringCount = IHU_INIT_COUNT;
	ringPtr = 0;
	if (!ringing)
	{
		player->playInit();
		ringing = true;
	}
}

void Phone::playRing(int callId)
{
#ifdef REN_DEBUG
	qWarning(QString("Phone::playRing(%1)").arg(callId));
#endif
//	qWarning("Phone::playRing()");
	if (!ringing)
	{
		ringPtr = 0;
		ringCount = IHU_RING_COUNT;
		ringing = true;
	}
}

void Phone::re_notification_sctp()
{
	sctp_notifier = new QSocketNotifier(sctp_sd,QSocketNotifier::Read,this);
	connect(sctp_notifier,SIGNAL(activated(int)),this,SLOT(newSCTPConnection(int)));
}
