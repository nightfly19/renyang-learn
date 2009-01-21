

/*
 *  IHU -- I Hear U, easy VoIP application using Speex and Qt
 *
 *  Copyright (C) 2003-2005 M. Trotta - <mrotta@users.sourceforge.net>
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

/*
 *  MOD -- Messenger On-the-Drive
 *
 *  Copyright (C) 2006 Kai-Fong Chou - <mod.mapleelpam at gmail.com>
 *
 *  http://mod.0rz.net
 *  http://mod.sf.net
 *
 */

#ifndef __TRANSMITTER_H__
#define __TRANSMITTER_H__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <qobject.h>
#include <qstring.h>
#include <qstrlist.h>
#include <qtimer.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "speex/speex.h"

#include "Recorder.h"
#include "Blowfish.h"
#include "Rsa.h"
#include "Packet.h"

class Transmitter: public QObject {
	Q_OBJECT
public:

	enum transmitter_status {
		TRANSMITTER_STATUS_NORMAL = 0, 
		TRANSMITTER_STATUS_START, 
		TRANSMITTER_STATUS_WAITING, 
		TRANSMITTER_STATUS_MUTE,
	};
	
	Transmitter(Recorder *, Rsa *);
	~Transmitter(void);
	void setup(int, int, int, int, float, int, int, int, int);
	void init(int);
	void dump(QString, QString);
	void start(int);
	int call(QString, int, int);
	int Callx(QStrList, int, int);
	void answer();
	void initRecorder();
	void end();
	void go();
	void stop();
	void ring(bool);
	void send(float *, int);
	void setThreshold(int);
	long getBytes();
	long getTotal();
	int getSocketFd(){ return s; }
	void enableCrypt(char *, int);
	void disableCrypt();
	void sendPacket(Packet *p , int str_number = 0 );
	void sendNewPacket(char*, int, char);
	void sendSpecialPacket(char*, int, char);
	void sendNamePacket(bool, char);
	void sendMsgPacket(QString);
	void sendAudioPacket(float *, int);
	void sendInitPacket();
	void sendResetPacket();
	void sendAnswerPacket();
	void sendClosePacket();
	void sendRefusePacket();
	void sendRingPacket();
	void newConnection(int, struct sockaddr_in, int);
	QString getIp();
	void changeStatus(transmitter_status);
	void prepare();
	void emitError(QString);
	void ledOn(bool);
	bool isWorking();

	void setTTL(int t){	ttl = t;	};
	int getTTL(){	return ttl;	};
private:
	Recorder *recorder;
	bool working;
	bool recording;
	bool ringing;
	bool transmitting;
	bool _enable_sctp;
	int s;
	struct sockaddr_in sa;
	socklen_t salen;
	char *out;
	int speak;
	int bufsize;
	float threshold;
	int port;
	FILE *outFile;
	void *state;
	int frame_size;
	SpeexBits bits;
	long bytes;
	long total;
	int ready;
	int toRead;
	float *readBuffer;
	unsigned int rate;
	Blowfish *blowfish;
	Rsa *rsa;
	char speexmode;
	float *buffer, *bufptr;
	float *prebuffer;
	transmitter_status status;
	QString myName;
	QTimer *timer;
	int protocol;
	int stoptx;
	int isSpeaking(float *, int);

	int ttl;

public slots:
	void processData(float *, int);
	void sendKeyRequestPacket();
	void sendKeyPacket();
	void sendRing();
	void sendRingReplyPacket();
signals:
	void finish();
	void error(QString);
	void message(QString);
	void ringMessage();
	void ledEnable(bool);
};

#endif

