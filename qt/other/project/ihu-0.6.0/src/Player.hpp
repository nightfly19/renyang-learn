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

#ifndef PLAYER_HPP
#define PLAYER_HPP

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <qobject.h>
#include <qstring.h>
#include <alsa/asoundlib.h>

#include <soundtouch/SoundTouch.h>

//Size: 2^15
#define SAMPLEBUFSIZE 65536

using namespace soundtouch;

class Player: public QObject {
	Q_OBJECT
public:
	Player();
	~Player();
	void start(unsigned int, int);
	void initAlsa(const char*);
	bool flush();
	void playAlsa(float *, int);
	void play(float*, int);
	void playShort(short*, int);
	void end();
	void setup(QString, int);
	bool ready();
	bool ready(int);
	void setAdr(bool, float, float, float);
	void update_frames(int);
	bool isWorking();
	float getDelay();
	void reset();
	void pause(bool);
	void playInit();
	void disableADR(bool);
private:
	int i;
	int err;
	unsigned int sample_rate;
	int frame_size;
	snd_pcm_t *playback_handle;
	snd_pcm_sframes_t delayNow;
	snd_pcm_state_t state;
	float delay;
	float minDelay;
	float maxDelay;
	float stretch;
	float tempoChange;
	float fdel;
	bool adr;
	bool ringing;
	int readyFrames;
	int prebuffer;
	int preframes;
	int prepackets;
	short *shortBuffer;
	bool working;
	QString interface;
	SoundTouch pSoundTouch;
	SAMPLETYPE sampleBuffer[SAMPLEBUFSIZE];

public slots:
	void put(float *, int);
	void callback();
signals:
	void suspend();
	void warning(QString);
};

#endif
