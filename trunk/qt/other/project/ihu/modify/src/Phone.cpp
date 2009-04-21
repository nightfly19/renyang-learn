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

Phone::Phone(int mc)
{
	maxcall = mc;
	calls = new Call*[maxcall];
	for (int i=0; i<maxcall; i++)
		calls[i] = NULL;
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

	connect( timer, SIGNAL(timeout()), this, SLOT(playCallback()) );
}

Phone::~Phone(void)
{
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

void Phone::waitCalls(int port, bool udp, bool tcp)
{
//	qWarning(QString("Phone::waitCalls()"));
	inport = port;

	if (udp)
	{
		if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
			throw Error(tr("Can't initalize socket! (socket())"));
		if (fcntl(sd, F_SETFL, O_NONBLOCK) == -1)
			throw Error(tr("fcntl can't set socket property\n") + strerror(errno));
	
		memset(&sa, 0, sizeof(sa));
		sa.sin_family = AF_INET;
		sa.sin_port = htons(port);
		sa.sin_addr.s_addr = htonl(INADDR_ANY);
	
		int opt = 1;
		setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt) );

		if (bind(sd, (struct sockaddr *) &sa, sizeof(sa)) == -1)
			throw Error(tr(QString("can't listen on UDP port %1 (%2)").arg(port).arg(strerror(errno))));
		
		notifier = new QSocketNotifier(sd, QSocketNotifier::Read, this);
		connect(notifier,SIGNAL(activated(int)),this, SLOT(newUDPConnection(int)));
	}
	if (tcp)
	{
		tcpserver = new TcpServer(this, port);
		connect( tcpserver, SIGNAL(newConnect(int)), this, SLOT(newTCPConnection(int)));
	}

	listening = true;
}

void Phone::newTCPConnection(int socket)
{
//	qWarning(QString("Phone::newTCPConnection(%1)").arg(socket));
	connections++;
	int callId = newCall();
	if (callId >= 0)
	{
		calls[callId]->start(socket, IHU_TCP);
		receivedCall(callId);
	}
	else
	{
		close(socket);
	}
}

void Phone::newUDPConnection(int socket)
{
//	qWarning(QString("Phone::newUDPConnection(%1)").arg(socket));
	connections++;
	delete notifier;
	notifier = NULL;
	int callId = newCall();
	if (callId >= 0)
	{
		calls[callId]->start(socket, IHU_UDP);
		receivedCall(callId);
	}
	else
	{
		close(socket);
	}
	waitCalls(inport, true, false);
}

void Phone::receivedCall(int callId)
{
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

int Phone::newCall()
{
	int callId = findFreeCall();
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

void Phone::stopWaiting()
{
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

int Phone::createCall()
{
	int newId = findFreeId();
	if (newId < 0)
	{
		throw Error(QString("max number of calls reached (%1)").arg(maxcall));
	}
	else
	{
		calls[newId] = new Call(newId, myName);
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
//	qWarning(QString("Phone::deleteCall(%1)").arg(callId));
	Call *old_call = calls[callId]; 
	calls[callId] = NULL;
	if (old_call)
		delete old_call; 
}

void Phone::connectedCall(int callId)
{
//	qWarning(QString("Phone::connectedCall(%1)").arg(callId));
	emit connectedCallSignal(callId);
}

void Phone::abortCall(int callId, QString text)
{
//	qWarning("Phone::abortCall(int, QString)");
	abortCall(callId);
	emit abortCallSignal(callId, text);
}

void Phone::abortCall(int callId)
{
//	qWarning("Phone::abortCall(int)");
	if (calls[callId])
		calls[callId]->error();
}

void Phone::callWarning(int callId, QString text)
{
	emit messageSignal(callId, text);
}

void Phone::receivedNewKey(int callId, QString text)
{
	emit newKeySignal(callId, text);
}

void Phone::stopCall(int callId)
{
//	qWarning("Phone::stopCall()");
	if (calls[callId])
		calls[callId]->stop();
}

void Phone::cancelCall(int callId)
{
//	qWarning(QString("Phone::cancelCall(%1)").arg(callId));
	checkSound();
	emit cancelCallSignal(callId);
}

void Phone::endCall(int callId)
{
//	qWarning("Phone::endCall()");
	if (calls[callId])
		calls[callId]->end();
}

void Phone::endAll()
{
	for (int i=0; i<maxcall; i++)
	{
		if (calls[i])
			calls[i]->end();
	}
}

void Phone::abortAll()
{
	for (int i=0; i<maxcall; i++)
	{
		if (calls[i])
		{
			calls[i]->error();
			calls[i]->stop();
		}
	}
}

void Phone::call(int callId, QString host, int port, int prot)
{
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
	else throw Error("invalid call ID");
}

void Phone::answerCall(int callId)
{
//	qWarning(QString("Phone::answerCall(%1) - connections=%2").arg(callId).arg(connections));
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
	player->setAdr(on, min, max, stretch);
}

void Phone::setupAgc(bool on, bool hw_on, bool sw_on, float step, float lev, const char* mixname)
{
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
	if (calls[callId])
		calls[callId]->mutePlayer(on);
}

void Phone::muteRecorder(int callId, bool on)
{
	if (calls[callId])
		calls[callId]->muteRecorder(on);
}

void Phone::disablePlayer(bool on)
{
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
	if (calls[callId])
		calls[callId]->enableRandomCrypt(len);
}

void Phone::enableCrypt(int callId, char *passwd, int len)
{
	if (calls[callId])
		calls[callId]->enableCrypt(passwd, len);
}

void Phone::enableDecrypt(int callId, char *passwd, int len)
{
	if (calls[callId])
		calls[callId]->enableDecrypt(passwd, len);
}

void Phone::disableDecrypt(int callId)
{
	if (calls[callId])
		calls[callId]->disableDecrypt();
}

void Phone::disableCrypt(int callId)
{
	if (calls[callId])
		calls[callId]->disableCrypt();
}

void Phone::setMyName(QString name)
{
	if(name.length() > IHU_MAX_NAME_LEN)
		name.truncate(IHU_MAX_NAME_LEN);
	myName = name;
	for (int i=0; i<maxcall; i++)
	{
		if (calls[i])
			calls[i]->setMyName(myName);
	}
}

void Phone::setup(int mode, int quality, int abr, int vbr, float vbr_quality, int complexity, int vad, int dtx, int txstop, int th, int ring_vol)
{
	bool restart = recording;
	if (restart)
		stopRecorder();

	if (enc_state)
		speex_encoder_destroy(enc_state);

	SpeexMode *spmode = NULL;
	
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
	
	enc_state = speex_encoder_init(spmode);

	speex_encoder_ctl(enc_state, SPEEX_SET_COMPLEXITY, &complexity);
	if (vbr)
	{
		speex_encoder_ctl(enc_state, SPEEX_SET_VBR, &vbr);
		speex_encoder_ctl(enc_state, SPEEX_SET_VBR_QUALITY, &vbr_quality);
	}
	else
	{
		speex_encoder_ctl(enc_state, SPEEX_SET_QUALITY, &quality);
		speex_encoder_ctl(enc_state, SPEEX_SET_VAD, &vad);
	}
	if (abr)
		speex_encoder_ctl(enc_state, SPEEX_SET_ABR, &abr);
	speex_encoder_ctl(enc_state, SPEEX_SET_DTX, &dtx);
	
	speex_encoder_ctl(enc_state, SPEEX_GET_SAMPLING_RATE, &rate);
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
	float input = (float) th;
	if (input <= -96.0)
		threshold = 0.0;
	else threshold = (float) pow(10.0, input/20.0);
	threshold *= 32768.0;
}

void Phone::setupRecorder(int driver, QString interface)
{
	recorder->setup(driver, interface);
}

void Phone::setupPlayer(QString interface, int prepackets)
{
	player->setup(interface, prepackets);
}

void Phone::ring(int callId, bool on)
{
//	qWarning("Phone::ring()");
	if (calls[callId])
		calls[callId]->sendRing(on);
}

void Phone::processAudioSamples(float *samples, int nsample)
{
	recBuffer = samples;
	while (nsample > 0)
	{
		toRead = frame_size - ready;
		if (nsample < toRead)
		{
			memcpy(bufptr, recBuffer, nsample*sizeof(float));
			ready += nsample;
			bufptr += nsample;
			nsample = 0;
			break;
		}
		else
		if (nsample >= toRead)
		{
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

void Phone::startRecorder()
{
//	qWarning("Phone::startRecorder()");
	switch (rec_status)
	{
		case RECORDER_STATUS_MUTE:
			break;
		default:
			if (!recording)
			{
				memset(prebuffer, 0x0, sizeof(float)*REC_BUFSIZE);
				memset(out, 0x0, MAXBUFSIZE);
				speak = 0;
				ready = toRead = 0;
				bufptr = buffer;
				recorder->start(rate);
				rec_status = RECORDER_STATUS_WAITING;
				recording = true;
				emit recorderSignal(true);
			}
			break;
	}
}

void Phone::stopRecorder()
{
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

void Phone::startPlayer()
{
//	qWarning("Phone::startPlayer()");
	switch (play_status)
	{
		case PLAYER_STATUS_MUTE:
			break;
		default:
			if (!playing)
			{
				player->start(play_rate, play_frame_size);
				play_status = PLAYER_STATUS_RUNNING;
				playing = true;
				emit playerSignal(true);
			}
			break;
	}
}

void Phone::stopPlayer()
{
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

bool Phone::isSpeaking(float *samples, int nsample)
{
	bool ret = false;
	float smp;
	for (int i=0; i<nsample; i++)
	{
		samples[i] *= amp;
		smp = fabs(samples[i]);
		if (smp > peak)
			peak = smp;
	}
	if (peak >= threshold)
		ret = true;
	return ret;
}

void Phone::sendAudioData(float *sample, int samples)
{
	if (isSpeaking(sample, samples))
	{
		if (rec_status == RECORDER_STATUS_WAITING)
			rec_status = RECORDER_STATUS_START;
		speak = stoptx; // To guarantee stream even for small speech pauses
	}
	switch (rec_status)
	{
		case RECORDER_STATUS_START:
			send(prebuffer, frame_size);
			rec_status = RECORDER_STATUS_RUNNING;
		case RECORDER_STATUS_RUNNING:
			if (speak > 0) {
				send(sample, samples);
				break;
			}
			else 
			{
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
	int tot = 0;
	speex_bits_reset(&enc_bits);
	if (enc_state)
	{
		speex_encode(enc_state, fsamples, &enc_bits);
		tot = speex_bits_write(&enc_bits, out, MAXBUFSIZE);
	}
	if (tot > 0) {
		try
		{
			for (int i=0; i<maxcall; i++)
			{
				if (calls[i])
					calls[i]->send(out, tot);
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
	for (int i=0; i<maxcall; i++)
	{
		if (calls[i] == NULL)
			return i;
	}
	return -1;
}

int Phone::findFreeCall()
{
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
	emit warningSignal(text);
}

QString Phone::getCallerName(int callId)
{
	QString ret = "";
	if (calls[callId])
		ret = calls[callId]->getCallerName();
	return ret;
}

QString Phone::getCallerIp(int callId)
{
	QString ret = "";
	if (calls[callId])
		ret = calls[callId]->getCallerIp();
	return ret;
}

long Phone::getCallTraffic(int callId)
{
	long ret = 0;
	if (calls[callId])
		ret = calls[callId]->getTraffic();
	return ret;
}

long Phone::getCallRX(int callId)
{
	long ret = 0;
	if (calls[callId])
		ret = calls[callId]->getTotalRX();
	return ret;
}

long Phone::getCallTX(int callId)
{
	long ret = 0;
	if (calls[callId])
		ret = calls[callId]->getTotalTX();
	return ret;
}

long Phone::getTotal()
{
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
//	qWarning(QString("Phone::checkSound() - call_number = %1").arg(call_number));
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
//	qWarning("Phone::cryptCall()");
	emit cryptedSignal(callId);
}

void Phone::dumpRXStream(int callId, QString filename)
{
	if (calls[callId])
		calls[callId]->dumpRXStream(filename);
}

void Phone::dumpTXStream(int callId, QString filename)
{
	if (calls[callId])
		calls[callId]->dumpTXStream(filename);
}

bool Phone::isDumpingRX(int callId)
{
	bool ret = false;
	if (calls[callId])
		ret = calls[callId]->isDumpingRX();
	return ret;
}

bool Phone::isDumpingTX(int callId)
{
	bool ret = false;
	if (calls[callId])
		ret = calls[callId]->isDumpingTX();
	return ret;
}

bool Phone::isRXActive(int callId)
{
	bool ret = false;
	if (calls[callId])
		ret = calls[callId]->isRXActive();
	return ret;
}

bool Phone::isTXActive(int callId)
{
	bool ret = false;
	if (calls[callId])
		ret = calls[callId]->isTXActive();
	return ret;
}

float Phone::getDelay()
{
	return player->getDelay();
}

bool Phone::isListening()
{
	return listening;
}

int Phone::getConnections()
{
	return connections;
}

int Phone::getCalls()
{
	return call_number;
}

void Phone::clearConnections()
{
	connections = 0;
}

int Phone::getPeak()
{
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
	peak = 0.f;
	return ret;
}

void Phone::playCallback()
{
	bool silence = true;
	balance = 2.f;
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
				player->put(playBuffer, play_frame_size);
			player->callback();
			break;
		default:
			break;
	}
}

void Phone::playInit(int callId)
{
//	qWarning("Phone::playInit()");
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
//	qWarning("Phone::playRing()");
	if (!ringing)
	{
		ringPtr = 0;
		ringCount = IHU_RING_COUNT;
		ringing = true;
	}
}
