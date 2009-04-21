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

#ifndef PHONE_HPP
#define PHONE_HPP

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <qobject.h>
#include <qtimer.h>
#include <qstring.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <qsocketnotifier.h>

#include <speex/speex.h>

#include "Call.hpp"
#include "Player.hpp"
#include "Recorder.hpp"
#include "TcpServer.hpp"

#include "ring_sounds.h"

class Phone: public QObject {
	Q_OBJECT
public:

	enum recorder_status {
		RECORDER_STATUS_RUNNING = 0,
		RECORDER_STATUS_START,
		RECORDER_STATUS_STOP,
		RECORDER_STATUS_WAITING,
		RECORDER_STATUS_MUTE
	};

	enum player_status {
		PLAYER_STATUS_RUNNING = 0,
		PLAYER_STATUS_STOP,
		PLAYER_STATUS_MUTE
	};

	Phone(int);
	~Phone();

	void resize(int);
	void waitCalls(int, bool, bool);
	void call(int, QString, int, int);
	void answerCall(int);
	void endAll();
	void endCall(int);
	void abortAll();
	void stopWaiting();
	int createCall();
	int newCall();
	void deleteCall(int);
	void ring(int, bool);
	void receivedCall(int);

	void send(float*, int);
	void sendAudioData(float *, int);

	void startRecorder();
	void stopRecorder();
	void startPlayer();
	void stopPlayer();

	void setup(int, int, int, int, float, int, int, int, int, int, int);
	void setupRecorder(int, QString);
	void setupPlayer(QString, int);
	void setupAdr(bool, float, float, float);
	void setupAgc(bool, bool, bool, float, float, const char*);
	void disableRecorder(bool);
	void disablePlayer(bool);
	void mutePlayer(int, bool);
	void muteRecorder(int, bool);

	void enableRandomCrypt(int, int);
	void enableDecrypt(int, char *, int);
	void enableCrypt(int, char *, int);
	void disableDecrypt(int);
	void disableCrypt(int);

	int getConnections();
	int getCalls();
	long getCallTraffic(int);
	long getCallTX(int);
	long getCallRX(int);
	long getTotal();
	QString getCallerName(int);
	QString getCallerIp(int);
	float getDelay();

	void dumpRXStream(int, QString);
	void dumpTXStream(int, QString);
	bool isDumpingRX(int);
	bool isDumpingTX(int);
	bool isRXActive(int);
	bool isTXActive(int);
	void setMyName(QString);
	void setThreshold(int);

	bool isSpeaking(float*, int);
	int findFreeId();
	int findFreeCall();
	void checkSound();
	bool isListening();
	void clearConnections();
	int getPeak();

private:

	recorder_status rec_status;
	player_status play_status;

	Recorder *recorder;
	Player *player;

	QTimer *timer;

	Call **calls;
	int maxcall;
	int call_number;
	int connections;

	TcpServer *tcpserver;
	struct sockaddr_in sa;
	QSocketNotifier* notifier;
	int sd;
	int inport;

	SpeexBits enc_bits;
	void *enc_state;

	float *recBuffer;
	int toRead;
	float *buffer, *bufptr;
	float *prebuffer;
	float *playBuffer;
	char *out;
	int frame_size;
	int ready;
	int stoptx;
	int speak;
	int rate;
	float threshold;
	float peak;
	int play_rate;
	int play_frame_size;
	float balance;

	bool recording;
	bool playing;
	bool listening;

	QString myName;

	bool agc;
	bool agc_hw;
	bool agc_sw;
	float agc_level;
	float agc_step;
	float amp;
	float agc_err;

	float ringBuffer[SIZE_RING_32];
	bool ringing;
	int ringPtr;
	int ringCount;

public slots:
	void warning(QString);
	void newUDPConnection(int);
	void newTCPConnection(int);
	void connectedCall(int);
	void cancelCall(int);
	void stopCall(int);
	void abortCall(int);
	void abortCall(int, QString);
	void callWarning(int, QString);
	void receivedNewKey(int, QString);
	void processAudioSamples(float*, int);
	void cryptCall(int);
	void playInit(int);
	void playRing(int);
	void playCallback();

signals:
	void warningSignal(QString);
	void messageSignal(int, QString);
	void newKeySignal(int, QString);
	void abortSignal(QString);
	void abortCallSignal(int, QString);
	void receivedCallSignal(int);
	void connectedCallSignal(int);
	void cancelCallSignal(int);
	void cryptedSignal(int);
	void newCallSignal(int);
	void recorderSignal(bool);
	void playerSignal(bool);
};

#endif
