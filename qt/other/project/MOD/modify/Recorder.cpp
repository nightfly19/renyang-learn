

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

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "Recorder.h"
#include "Error.h"

#define ALSA_REC_BUFSIZE 1024
#define TICKTIME 5

Recorder::Recorder()
{
	capture_handle = NULL;
	tempDriver = driver = REC_DRIVER_ALSA;
	working = false;
	if ((samples = (short *) malloc(ALSA_REC_BUFSIZE*sizeof(short)))==NULL)
		throw Error(Error::DRTA_ERR_MEMORY);
	if ((audioBuffer = (float *) malloc(REC_BUFSIZE*sizeof(float)))==NULL)
		throw Error(Error::DRTA_ERR_MEMORY);
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(alsa_callback()));
#ifdef HAVE_LIBJACK
	jack_handle = NULL;
	jackdata = (jack_default_audio_sample_t *) malloc(REC_BUFSIZE * sizeof(jack_default_audio_sample_t));
	if (jackdata == NULL)
		throw Error(Error::DRTA_ERR_MEMORY);
#endif
}

Recorder::~Recorder(void)
{
	end();
	if (samples)
		free(samples);
	if (audioBuffer)
		free(audioBuffer);
#ifdef HAVE_LIBJACK
	if (jackdata)
		free(jackdata);
#endif
}

void Recorder::setup(int driv, QString interf)
{
	tempDriver = driv;
	interface = interf;
}

void Recorder::start(unsigned int rate)
{
	driver = tempDriver;
	switch(driver)
	{
		case REC_DRIVER_ALSA:
			initAlsa(rate, interface.ascii());
			break;
		case REC_DRIVER_JACK:
#ifdef HAVE_LIBJACK
			initJack();
#else
			throw Error(tr("DRTA was compiled without Jack support for recorder"));
#endif
			break;
	}
	working = true;
}

void Recorder::initAlsa(unsigned int rate, const char* interf)
{
	snd_pcm_hw_params_t *hw_params;
	
	if ((err = snd_pcm_open (&capture_handle, interf, SND_PCM_STREAM_CAPTURE, SND_PCM_NONBLOCK)) < 0)
		throw Error(tr(QString("ALSA: cannot open audio device for capture (%1)").arg(snd_strerror(err))));
	
	if ((err = snd_pcm_nonblock(capture_handle, 1)) < 0)
		throw Error(tr(QString("ALSA: cannot set nonblock (%1)").arg(snd_strerror(err))));
	
	if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0)
		throw Error(tr(QString("ALSA: cannot allocate hardware parameter structure (%1)").arg(snd_strerror(err))));
				
	if ((err = snd_pcm_hw_params_any (capture_handle, hw_params)) < 0)
		throw Error(tr(QString("ALSA: cannot initialize hardware parameter structure (%1)").arg(snd_strerror(err))));

	if ((err = snd_pcm_hw_params_set_access (capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
		throw Error(tr(QString("ALSA: cannot set access type (%1)").arg(snd_strerror(err))));

	if ((err = snd_pcm_hw_params_set_format (capture_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0)
		throw Error(tr(QString("ALSA: cannot set sample format (%1)").arg(snd_strerror(err))));

	if ((err = snd_pcm_hw_params_set_rate (capture_handle, hw_params, rate, 0)) < 0)
		throw Error(tr(QString("ALSA: cannot set sample rate (%1)").arg(snd_strerror(err))));

	if ((err = snd_pcm_hw_params_set_channels (capture_handle, hw_params, 1)) < 0)
		throw Error(tr(QString("ALSA: cannot set channel count (%1)").arg(snd_strerror(err))));

	snd_pcm_uframes_t bufsize = ALSA_REC_BUFSIZE;
	switch(rate)
	{
		case 8000:
			bufsize >>= 2;
			break;
		case 16000:
			bufsize >>= 1;
			break;
		case 32000:
			bufsize = ALSA_REC_BUFSIZE;
			break;
		default:
			throw Error(tr(QString("Recorder: wrong sample rate %1").arg(rate)));
	}
	
	if ((err = snd_pcm_hw_params_set_buffer_size_near(capture_handle, hw_params, &bufsize)) < 0)
		throw Error(tr(QString("ALSA: cannot set buffer size (%1)").arg(snd_strerror(err))));
	
	if ((err = snd_pcm_hw_params (capture_handle, hw_params)) < 0)
		throw Error(tr("ALSA: cannot set parameters (%1)").arg(snd_strerror(err)));

	snd_pcm_hw_params_free (hw_params);
	
	snd_pcm_sw_params_t *sw_params;
	if ((err = snd_pcm_sw_params_malloc (&sw_params)) < 0)
		throw Error(tr(QString("ALSA: cannot allocate hardware parameter structure (%1)").arg(snd_strerror(err))));
				
	if ((err = snd_pcm_sw_params_current (capture_handle, sw_params)) < 0)
		throw Error(tr(QString("ALSA: cannot initialize hardware parameter structure (%1)").arg(snd_strerror(err))));

	if ((err = snd_pcm_sw_params_set_avail_min(capture_handle, sw_params, bufsize/2)) < 0)
		throw Error(tr(QString("ALSA: cannot set access type (%1)").arg(snd_strerror(err))));
	
	if ((err = snd_pcm_sw_params_set_stop_threshold(capture_handle, sw_params, bufsize*4)) < 0)
		throw Error(tr(QString("ALSA: cannot set access type (%1)").arg(snd_strerror(err))));
	
	if ((err = snd_pcm_sw_params (capture_handle, sw_params)) < 0)
		throw Error(tr(QString("ALSA: cannot set parameters (%1)").arg(snd_strerror(err))));
	
	if ((err = snd_pcm_prepare (capture_handle)) < 0)
		throw Error(tr(QString("ALSA: cannot prepare audio interface for use (%1)").arg(snd_strerror(err))));
	
	timer->start(TICKTIME, false);
	
	if ((err = snd_pcm_start(capture_handle)) < 0)
		throw Error(tr(QString("ALSA: cannot start interface (%1)").arg(snd_strerror(err))));
}

bool Recorder::isWorking()
{
	return working;
}

void Recorder::end(void)
{
	timer->stop();
	working = false;
	if (capture_handle)
	{
		snd_pcm_drop(capture_handle);
		snd_pcm_close(capture_handle);
	}
	capture_handle = NULL;
#ifdef HAVE_LIBJACK
	endJack();
#endif
}

void Recorder::alsa_callback()
{
	err = snd_pcm_readi (capture_handle, samples, ALSA_REC_BUFSIZE);
	if (err < 0)
	{
#ifdef DRTA_DEBUG
		fprintf (stderr, "ALSA: RECORDER Error: %s\n", snd_strerror (err));
#endif
		snd_pcm_state_t pcm_state = snd_pcm_state (capture_handle);
		if (pcm_state == SND_PCM_STATE_XRUN) // An XRUN occurred
		{
			snd_pcm_prepare (capture_handle);
			snd_pcm_start(capture_handle);
		}
	}
	if (err > 0)
	{
		for (int i=0; i < err; i++)
			audioBuffer[i] = (float) samples[i];
		emit data(audioBuffer, err);
	}
}

#ifdef HAVE_LIBJACK

void Recorder::initJack()
{
	fflush(stderr);
	if ((jack_handle = jack_client_new("drta_recorder")) == NULL)
		throw Error(tr("JACK: cannot connect to server. (is Jack running?)"));
	if (jack_set_process_callback(jack_handle, capture_callback, (void *)this))
		throw Error(tr("JACK: cannot set process callback."));
	jack_in = jack_port_register(jack_handle, "Input", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
	if (jack_in == NULL)
		throw Error(tr("JACK: cannot register Input port"));
	if (jack_activate(jack_handle))
		throw Error(tr("JACK: cannot activate client"));
	if (jack_connect(jack_handle, "alsa_pcm:capture_1", "drta_recorder:Input"))
		emit warning("Couldn't connect DRTA recorder with Jack port: alsa_pcm:capture_1");
}
 
void Recorder::endJack()
{
	if (jack_handle)
	{
		if (jack_deactivate(jack_handle) == 0)
			jack_client_close(jack_handle);
	}
	jack_handle = NULL;
}

int Recorder::capture_callback(jack_nframes_t nframes, void *arg)
{
	Recorder *r = (Recorder *)arg;
	return (r->jack_capture(nframes));
}

int Recorder::jack_capture(jack_nframes_t nframes)
{
	int frames = (int) nframes;
	jack_default_audio_sample_t *buf;

	if (frames > REC_BUFSIZE)
	{
		qWarning("Warning: the jack buffer is too large. Please reduce to max " + REC_BUFSIZE);
		nframes = REC_BUFSIZE;
		frames = (int) nframes;
	}

	buf = (jack_default_audio_sample_t *)jack_port_get_buffer(jack_in, nframes);
	memcpy(jackdata, buf, sizeof(jack_default_audio_sample_t) * nframes);
	
	for (int i=0; i < frames; i++)
		audioBuffer[i] = (float) (32767.0 * jackdata[i]);
	
	emit data(audioBuffer, frames);
	return 0;
}

#endif
