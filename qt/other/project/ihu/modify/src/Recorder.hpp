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

#ifndef SOUND_HPP
#define SOUND_HPP

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qobject.h>
#include <qtimer.h>
#include <alsa/asoundlib.h>

#ifdef HAVE_LIBJACK
#include <jack/jack.h>
#endif

#define REC_DRIVER_ALSA 0
#define REC_DRIVER_JACK 1

#define REC_BUFSIZE 4096

class Recorder : public QObject {
	Q_OBJECT
public:
	Recorder();
	~Recorder(void);
	void setup(int, QString);
	void initAlsa(unsigned int, const char*);
	void start(unsigned int);
	void end(void);
	bool isWorking();
	void adjustVolume(float);
	void setupAgc(bool, const char*);
#ifdef HAVE_LIBJACK
	void initJack();
	void endJack();
#endif
private:
	int err;
	snd_pcm_t *capture_handle;
	snd_mixer_t *mhandle;
	snd_mixer_elem_t *elem;
	short *samples;
	float *audioBuffer;
	int driver;
	int tempDriver;
	bool working;
	QString interface;
	QTimer *timer;
	float volume;
	long vol_min;
	long vol_max;
	long vol;
#ifdef HAVE_LIBJACK
	jack_client_t *jack_handle;
	jack_port_t *jack_in;
	jack_default_audio_sample_t *jackdata;
	static int capture_callback(jack_nframes_t nframes, void *arg);
	int jack_capture(jack_nframes_t nframes);
#endif
signals:
        void data(float*, int);
	void warning(QString);
private slots:
        void alsa_callback();
};

#endif

