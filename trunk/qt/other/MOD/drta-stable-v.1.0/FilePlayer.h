

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

/*加點東西*/
#ifndef FILEPLAYER_H
#define FILEPLAYER_H

#include <stdio.h>
#include <stdlib.h>
/* QT */
#include <qobject.h>
#include <qstring.h>
#include <qtimer.h>
/* 特別的 libraries */
#include <speex/speex.h>

#include "Player.h"

#define DRTA_INFO_MODE_ULTRAWIDE 0xc0
#define DRTA_INFO_MODE_WIDE 0x80
#define DRTA_INFO_MODE_NARROW 0x40


class FilePlayer: public QObject
{
	Q_OBJECT
public:
		enum fileplayer_status {
			FP_STATUS_NORMAL = 0,
			FP_STATUS_MUTE
		};
		enum signal_type {
			SIGNAL_FINISH = 0,
			SIGNAL_RINGREPLY,
			SIGNAL_SENDNEWKEY,
			SIGNAL_KEYREQUEST,
			SIGNAL_NEWKEY,
		};

		FilePlayer(Player*);
		~FilePlayer();

		void changeMode(char);
		void initPlayer(fileplayer_status);
		void reset();

		void playData(char *, int);

		void emitSignal(signal_type);

		/* File Handler */
		void startFile();
		void seekFile(int);
		void flush();

		void putData(char*,int);
public slots:
		void readFile();
		void checkPlayer();


private:
		/* QT's */
		QTimer *timer;
		QTimer *fileTimer;
		QTimer *checkTimer;

		/* for speex */
		SpeexBits bits;
		char speexmode;
		void *state; // ?!

		/* drta local class */
		Player *player;

		/* buffers (remember destroy) */
		char *inputBuffer;
		char *streamBuffer;
		float *outBuffer;

		short *ring_buffer;
		int ring_size;

		/* read file */
		FILE *inFile;
		int read_size;

		/* class local data*/
		fileplayer_status status;
		bool flushing;


		/* for sound control */
		int rate;
		int frame_size;
		
signals:
		void finish();
		void error(QString);
		void keyRequest();
		void sendNewKey();
		void newKey(QString);
		void ringReply();
		void message(QString);
		void warning(QString);
		void fileProgress(int);
		void ledEnable(bool);

};


#endif 
