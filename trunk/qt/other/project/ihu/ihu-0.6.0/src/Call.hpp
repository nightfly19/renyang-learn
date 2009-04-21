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

#ifndef CALL_HPP
#define CALL_HPP

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <qobject.h>
#include <qstring.h>
#include <qtimer.h>

#include <speex/speex.h>

#include "Rsa.h"
#include "Receiver.hpp"
#include "Transmitter.hpp"

class Call: public QObject {
	Q_OBJECT
public:

	Call(int, QString);
	~Call(void);
	void start(int, int);
	int getId();
	QString getCallerName();
	QString getCallerIp();
	void enableRandomCrypt(int);
	void enableCrypt(char *, int);
	void enableDecrypt(char *, int);
	void disableCrypt();
	void disableDecrypt();
	void answer();
	void end();
	void stop();
	void error();
	void sendRing(bool);
	void call(QString, int, int);
	long getTraffic();
	long getTotalTX();
	long getTotalRX();
	long getTotal();
	void send(char*, int);
	void muteRecorder(bool);
	void mutePlayer(bool);
	void dumpRXStream(QString);
	void dumpTXStream(QString);
	bool isDumpingRX();
	bool isDumpingTX();
	bool isRXActive();
	bool isTXActive();
	bool isFree();
	void setMyName(QString);
	void updateFrames(int);
	void putData(float*, int);
	bool isRecording();

private:
	int id;
	int sd;

	Rsa *rsa;
	Receiver *receiver;
	Transmitter *transmitter;

	SpeexBits bits;
	void *dec_state;
	float *outBuffer;
	float *soundBuffer;
	int frame_size;
	int readyFrames;

	int inport;
	bool udp;
	bool tcp;
	bool recording;
	bool active;
	bool muteRec;
	bool mutePlay;
	bool callFree;
	bool aborted;
	QTimer *stopTimer;

public slots:
	void newConnection(int, int, struct sockaddr_in);
	void sendKeyRequest();
	void sendKey();
	void receivedNewKey(QString);
	void stopCall();
	void message(QString);
	void warning(QString);
	void abortCall(QString);
	void ringMessage();
	void startRecorder();
	void connected();
	void close();
	void decodeAudioData(char*, int);
	bool playData(float*, int);
	bool mixData(float*, int, float);
	void playRing();
	void playInit();

signals:
	void connectedSignal(int);
	void receivedSignal(int);
	void cancelCallSignal(int);
	void abortSignal(int, QString);
	void newKeySignal(int, QString);
	void warningSignal(int, QString);
	void cryptedSignal(int);
	void ringSignal(int);
	void initSignal(int);
};

#endif
