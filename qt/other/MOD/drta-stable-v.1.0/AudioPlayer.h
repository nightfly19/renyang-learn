


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

#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

/* for global headers */
#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>

#include <qobject.h>
#include <qtimer.h>
#include <qstring.h>

#include <soundtouch/SoundTouch.h> // debian predefine

/* for local headers */

/* 設定使用那 個sound driver */
#define PLAY_DRIVER_JACK 1
#define PLAY_DRIVER_ALSA 0

using namespace soundtouch;

#ifdef HAVE_LIBJACK
#include <jack/jack.h>
#endif

class AudioPlayer: public QObject {
	Q_OBJECT
public:
	AudioPlayer();
	~AudioPlayer(void);
	void init(unsigned int, int);
	void initAlsa(const char*);

	/* File Handler */
	bool flush();
	void playAlsa(float *, int);
	void play(float *, int);
	void end();
	void setup(int, QString, int);
	bool ready();
	bool ready(int);

	void setAdr(bool, float, float, float);
	void setAgc(bool, float, float, float, float);

	void ring(short *, int);
	void update_frames(int frames);
	bool isWorking();
	void reset();
private:
	int i;
	int err;
	unsigned int sample_rate;
	int frame_size;
	snd_pcm_t *playback_handle;
	snd_pcm_sframes_t delayNow;
	int driver;			/* ALSA(1) or JACK(0) */
	int tempDriver; 		/* ALSA(1) or JACK(0) */

	float ringvolume;
	float delay;
	float minDelay;
	float maxDelay;
	float stretch;
	float tempoChange;		/* for soundtouch */
	float fdel;
	/* 由UI or Command line 設進來*/
	bool adr;
	bool agc;

	float level;
	float amp;
	float minamp;
	float maxamp;
	float agc_err;
	float agc_step;
	int readyFrames;
	int prebuffer;
	int preframes;
	QTimer *timer;
	bool working;
	QString interface;

	/* Buffers */
	float *audioBuffer;
	SAMPLETYPE *sampleBuffer;
	short *shortBuffer;

	SoundTouch *pSoundTouch;

#ifdef HAVE_LIBJACK /* For Jack libraries*/
public:
	void playJack(float *, int);
	void initJack();
	void endJack();
private:
	jack_client_t *jack_handle;
	jack_port_t *jack_out;
	jack_default_audio_sample_t *jackdata;
	static int jack_callback(jack_nframes_t , void *);
	int jack_play(jack_nframes_t);
#endif


	/* QT Handler */
public slots:
	void put(float *, int);
	void alsa_callback();
signals:
	void suspend();
	void warning(QString);
};

#endif

