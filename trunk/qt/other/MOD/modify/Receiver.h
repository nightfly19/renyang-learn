

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

#ifndef RECEIVER_H
#define RECEIVER_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <qobject.h>
#include <qstring.h>
#include <qtimer.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <qsocketnotifier.h>


#include "speex/speex.h"

#include "AudioPlayer.h"
#include "Config.h"
#include "Packet.h"
#include "Blowfish.h"
#include "Rsa.h"


class SocketSctpServer;
class TcpServer;

class Receiver: public QObject {
	Q_OBJECT
public:

	enum receiver_status {
		RECEIVER_STATUS_NORMAL = 0,
		RECEIVER_STATUS_MUTE
	};
	
	enum stream_status {
		STREAM_OK = 0,
		STREAM_OUT_OF_SYNC,
		STREAM_MISSING_DATA,
		STREAM_DATA,
		STREAM_READ_DATA,
		STREAM_PLAYER_NOT_READY
	};
	
	enum signal_type {
		SIGNAL_FINISH = 0,
		SIGNAL_RINGREPLY,
		SIGNAL_SENDNEWKEY,
		SIGNAL_KEYREQUEST,
		SIGNAL_NEWKEY,
	};
	
	Receiver(AudioPlayer *, Rsa *);
	//Receiver(AudioPlayer *);
	~Receiver(void);
	void reset();
	void resetStream();
	void changeMode(char);
	void initAudioPlayer(receiver_status);
	void dump(QString);
	void waitConnection();
	void Listen(int, bool, bool, bool, bool);
	void putData(char *, int);
	void processData();
	void processPacket(Packet *);
	void playData(char *, int);
	void playFile(QString file);
	void stopFile();
	long getBytes();
	long getTotal();
	void enableDecrypt(char *passwd, int len);
	void disableDecrypt();
	int getCalls();
	int getConnections();
	void resetCalls();
	void close();
	void stop();
	void go();
	void swap();
	void startFile();
	void changeStatus(receiver_status);
	void ring(int);
	void seekFile(int);
	void ledOn(bool);
	void noDecrypt();
	QString getIp();
	QString getCallerName();
	bool refused();
	bool replied();
	void emitError(QString);
	void emitSignal(signal_type);
	void flush();
private:
	AudioPlayer *player;
	int s;
	receiver_status status;
	stream_status sync;
	struct sockaddr_in sa;
	struct sockaddr_in ca;
	socklen_t calen;
	int port;
	QSocketNotifier* notifier;
	FILE *streamFile;
	FILE *inFile;
	char *inputBuffer;
	char *streamBuffer;
	char *streamPtr;
	float *outBuffer;
	SpeexBits bits;
	void *state;
	int frame_size;
	long bytes;
	long total;
	long packets;
	int rate;
	int streamLen;
	Blowfish *blowfish;
	Rsa *rsa;
	char speexmode;
	TcpServer *tcpserver;
	SocketSctpServer *sctpserver;
	short *ring_buffer;
	int ring_size;
	bool working;
	bool realtime;
	bool listening;
	bool fromFile;
	bool nodecrypt;
	bool refuse;
	bool reply;
	bool connected;
	bool newconnected;
	bool halfconnected;
	bool flushing;
	bool playing;
	int calls;
	int connects;
	QTimer *timer;
	QTimer *fileTimer;
	QTimer *checkTimer;
	QString callerName;
	int protocol;
	int read_size;
public slots:
	void readFile();
	void receive(void);
	void end();
	void start(int, int);
	void checkAudioPlayer();
	void goRing();
	void checkConnection();
	void newConnectionTCP(int);
	void newConnectionSCTP(int);
private slots:
	void emitSctpEvent(void * inmessage);	
signals:
	void finish();
	void error(QString);
	void keyRequest();
	void sendNewKey();
	void newKey(QString);
	void ringReply();
	void message(QString);
	void warning(QString);
	void newSocket(int,int,struct sockaddr_in);
	void fileProgress(int);
	void ledEnable(bool);

	void sigRecvMsg(QString);
	void sigRecvFrom(QString);
	void sigSctpEvent(QString);
	void sigRemoteTerminate();
	void sigPrSendFailed();
};

#endif

