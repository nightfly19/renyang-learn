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

#ifndef FILEPLAYER_HPP
#define FILEPLAYER_HPP

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <qobject.h>
#include <qstring.h>
#include <qtimer.h>

#include <speex/speex.h>

#include "Config.h"
#include "Player.hpp"
#include "Packet.h"
#include "Rsa.h"
#include "Blowfish.h"
#include "Ihu2Spx.h"

class FilePlayer: public QObject {
	Q_OBJECT
public:

	enum stream_status {
		STREAM_OK = 0,
		STREAM_OUT_OF_SYNC,
		STREAM_MISSING_DATA,
		STREAM_READ_DATA,
		STREAM_PLAYER_NOT_READY,
		STREAM_DONE
	};
	
	enum signal_type {
		SIGNAL_FINISH = 0,
		SIGNAL_RINGREPLY,
		SIGNAL_SENDNEWKEY,
		SIGNAL_KEYREQUEST,
		SIGNAL_NEWKEY,
	};
	
	FilePlayer();
	~FilePlayer(void);

	void reset();
	void resetStream();
	void moveStream();
	void putData(char *, int);
	void processData();
	bool processPacket(Packet *);
	void playData(char *, int);
	void playFile(QString);
	void convertFile(QString);
	void openFile(QString);
	void stopFile();
	long getBytes();
	long getTotal();
	void enableDecrypt(char *, int);
	void disableDecrypt();
	void stop();
	void go();
	void pause();
	void resume();
	void seekFile(int);
	void noDecrypt();
	QString getCallerName();
	void emitError(QString);
	void emitSignal(signal_type);
	void flush();
	int getProgress();
	void setupPlayer(QString);

private:
	Player *player;
	Ihu2Spx *ihu2spx;
	stream_status sync;
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
	int rate;
	int streamLen;
	Blowfish *blowfish;
	Rsa *rsa;
	short *ring_buffer;
	int ring_size;
	bool working;
	bool nodecrypt;
	bool spx;
	bool fast;
	QTimer *timer;
	QTimer *fileTimer;
	QString callerName;

public slots:
	void readFile();
	void end();
	void checkPlayer();
	void playCallback();

signals:
	void finish();
	void error(QString);
	void keyRequest();
	void warning(QString);
};

#endif

