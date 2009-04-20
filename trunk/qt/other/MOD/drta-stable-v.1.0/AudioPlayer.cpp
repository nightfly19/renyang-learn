

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

#include <math.h>

#include "AudioPlayer.h"
#include "Error.h"

#define MAXBUFSIZE 65536
#define TICKTIME 5
#define PREBUFFER_FRAMES 4

#define VOL_FACTOR 4.0
//
AudioPlayer::AudioPlayer()
{
	tempDriver = driver = PLAY_DRIVER_ALSA; /* predefine alsa rather then jack */
	playback_handle = NULL;
	adr = false;
	agc = false;
	working = false;
	readyFrames = 0;
	pSoundTouch = new SoundTouch();
	pSoundTouch->setChannels(1);
	pSoundTouch->setSetting(SETTING_USE_QUICKSEEK, 1);
	pSoundTouch->setSetting(SETTING_USE_AA_FILTER, 0);
	sampleBuffer = new SAMPLETYPE [MAXBUFSIZE];
	timer = new QTimer(this);
	if ((shortBuffer = (short *) malloc(MAXBUFSIZE*sizeof(short)))==NULL)
		throw Error(Error::DRTA_ERR_MEMORY);
	if ((audioBuffer = (float *) malloc(MAXBUFSIZE*sizeof(float)))==NULL)
		throw Error(Error::DRTA_ERR_MEMORY);

#ifdef HAVE_LIBJACK
	jack_handle = NULL;
	jackdata = (jack_default_audio_sample_t *)malloc(MAXBUFSIZE * sizeof(jack_default_audio_sample_t));
	if (jackdata==NULL)
		throw Error(Error::DRTA_ERR_MEMORY);
#endif
	connect(timer, SIGNAL(timeout()), this, SLOT(alsa_callback()));
}

AudioPlayer::~AudioPlayer(void)
{
	end();
	if (audioBuffer)
		free(audioBuffer);
	if (shortBuffer)
		free(shortBuffer);
#ifdef HAVE_LIBJACK
	if (jackdata)
		free(jackdata);
#endif
	delete pSoundTouch;
	delete sampleBuffer;
}

void AudioPlayer::setup(int driv, QString interf, int vol)
{
	tempDriver = driv;
	interface = interf;
	vol = -vol + 1;
	if (vol > 0)
		ringvolume = 0.0;
	else
		ringvolume = powf(VOL_FACTOR, (float) (vol));
}

bool AudioPlayer::isWorking()
{
	return working;
}

void AudioPlayer::init(unsigned int rate, int fsize)
{
	sample_rate = rate;
	frame_size = fsize;
	readyFrames = 0;
	preframes = PREBUFFER_FRAMES*frame_size;
	prebuffer = preframes;
	
	driver = tempDriver;
	
	switch(driver)
	{
		case PLAY_DRIVER_ALSA:
			initAlsa(interface.ascii());
			break;
		case PLAY_DRIVER_JACK:
#ifdef HAVE_LIBJACK
			initJack();
#else
			throw Error(tr("DRTA was compiled without Jack support for player"));
#endif
			break;
	}

	pSoundTouch->clear();
	pSoundTouch->setSampleRate(rate);

	delay = 0.f;
	amp = 1.0;
	
	working = true;
}

void AudioPlayer::reset()
{
	readyFrames = 0;
	switch(driver)
	{
		case PLAY_DRIVER_ALSA:
			if (playback_handle)
			{
				snd_pcm_drop(playback_handle);
				snd_pcm_prepare (playback_handle);
			}
			break;
	}
}

void AudioPlayer::initAlsa(const char *interf)
{
	snd_pcm_hw_params_t *hw_params;
	
	if ((err = snd_pcm_open (&playback_handle, interf, SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK)) < 0)
		throw Error(tr(QString("ALSA: cannot open audio device for playback (%1)").arg(snd_strerror(err))));
	
	if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0)
		throw Error(tr(QString("ALSA: cannot allocate hardware parameter structure (%1)").arg(snd_strerror(err))));
	
	if ((err = snd_pcm_hw_params_any (playback_handle, hw_params)) < 0)
		throw Error(tr(QString("ALSA: cannot initialize hardware parameter structure (%1)").arg(snd_strerror(err))));

	if ((err = snd_pcm_hw_params_set_access (playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
		throw Error(tr(QString("ALSA: cannot set access type (%1)").arg(snd_strerror(err))));

	if ((err = snd_pcm_hw_params_set_format (playback_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0)
		throw Error(tr(QString("ALSA: cannot set sample format (%1)").arg(snd_strerror(err))));
	
	if ((err = snd_pcm_hw_params_set_channels (playback_handle, hw_params, 1)) < 0)
		throw Error(tr(QString("ALSA: cannot set channel count (%1)").arg(snd_strerror(err))));
	
	if ((err = snd_pcm_hw_params_set_rate (playback_handle, hw_params, sample_rate, 0)) < 0)
		throw Error(tr(QString("ALSA: cannot set sample rate (%1)").arg(snd_strerror(err))));

	if ((err = snd_pcm_hw_params (playback_handle, hw_params)) < 0)
		throw Error(tr(QString("ALSA: cannot set hw parameters (%1)").arg(snd_strerror(err))));

	snd_pcm_hw_params_free (hw_params);
	
	snd_pcm_sw_params_t *sw_params;
	if ((err = snd_pcm_sw_params_malloc (&sw_params)) < 0)
		throw Error(tr(QString("ALSA: cannot allocate hardware parameter structure (%1)").arg(snd_strerror(err))));
				
	if ((err = snd_pcm_sw_params_current (playback_handle, sw_params)) < 0)
		throw Error(tr(QString("ALSA: cannot initialize hardware parameter structure (%1)").arg(snd_strerror(err))));

	if ((err = snd_pcm_sw_params_set_avail_min(playback_handle, sw_params, frame_size)) < 0)
		throw Error(tr(QString("ALSA: cannot set access type (%1)").arg(snd_strerror(err))));
	
	if ((err = snd_pcm_sw_params (playback_handle, sw_params)) < 0)
		throw Error(tr(QString("ALSA: cannot set parameters (%1)").arg(snd_strerror(err))));
	
	if ((err = snd_pcm_prepare (playback_handle)) < 0)
		throw Error(tr(QString("ALSA: cannot prepare audio interface for use (%1)").arg(snd_strerror(err))));
		
	timer->start(TICKTIME, false);
}

void AudioPlayer::setAdr(bool on, float min, float max, float stch)
{
	adr = on;
	if (adr)
	{
		minDelay = min;
		maxDelay = max;
		tempoChange = stch;
		stretch = 0.f;
		pSoundTouch->clear();
	}
}

void AudioPlayer::setAgc(bool on, float step, float lev, float min, float max)
{
	agc = on;
	agc_step = step;
	level = lev*lev;
	agc_err = 0.0;
	minamp = min;
	maxamp = max;
}

void AudioPlayer::play(float *buffer, int frames)
{
	if (ready(frames))
	{
		for(i=0;i<frames;i++)
		{
			if (buffer[i] > 32767.f)
				buffer[i] = 32767.f;
			if (buffer[i] < -32768.f)
				buffer[i] = -32768.f;
		}
		memcpy(audioBuffer + readyFrames, buffer, frames*sizeof(float));
		switch(driver)
		{
			case PLAY_DRIVER_ALSA:
				for (i=0; i<frames; i++)
					shortBuffer[readyFrames+i] = (short) buffer[i];
				break;
			case PLAY_DRIVER_JACK:
#ifdef HAVE_LIBJACK
				for (i=0; i<frames; i++)
					jackdata[readyFrames+i] = (jack_default_audio_sample_t) (buffer[i]/32768.f);
#endif
				break;
		}
		readyFrames += frames;
	}
}

void AudioPlayer::update_frames(int frames)
{
	readyFrames -= frames;
	if (readyFrames < 0)
		readyFrames = 0;
	if (readyFrames > 0)
	{
		memcpy(audioBuffer, audioBuffer+frames, sizeof(float)*readyFrames);
		switch(driver)
		{
			case PLAY_DRIVER_ALSA:
				memcpy(shortBuffer, shortBuffer+frames, sizeof(short)*readyFrames);
				break;
			case PLAY_DRIVER_JACK:
#ifdef HAVE_LIBJACK
				memcpy(jackdata, jackdata+frames, sizeof(jack_default_audio_sample_t)*readyFrames);
#endif
				break;
		}
	}
}

bool AudioPlayer::ready(int frames)
{
	int remaining = MAXBUFSIZE - readyFrames;
	if (remaining < frames)
	{
		return false;
	}
	return true;
}

bool AudioPlayer::ready()
{
	return ready(MAXBUFSIZE>>4);
}

void AudioPlayer::alsa_callback()
{
	if (readyFrames > prebuffer)
	{
		err = snd_pcm_wait(playback_handle, 1000);
		if (err < 0) // An XRUN occurred
		{
			prebuffer = preframes;
			delay = (float) preframes;
			delay /= sample_rate;
			snd_pcm_prepare (playback_handle);
#ifdef DRTA_DEBUG
			qWarning("ALSA: PLAYER XRUN!");
#endif
		}
		else
		{
			if (adr)
			{
				if (snd_pcm_delay(playback_handle, &delayNow) == 0)
				{
					fdel = (float) delayNow;
					delay = 0.5 * delay + 0.5 * (fdel/sample_rate);
				}
			}
			err = snd_pcm_writei (playback_handle, shortBuffer, readyFrames);
			if (err > 0)
			{
				update_frames(err);
				prebuffer = 0;
			}
		}
	}
}
void AudioPlayer::put(float *buffer, int frames)
{
	int nSamples = frames;
	if (agc)
	{
		float sample, max = 0.f;
		for (i=0; i<frames; i++)
		{
			buffer[i] *= amp;
			sample = fabs(buffer[i])/32768.f;
			if (sample > max)
				max = sample;
		}
		agc_err = 0.9*agc_err + 0.1*(level-max);
		amp += agc_err*agc_step;
		if (amp < minamp)
			amp = minamp;
		else if (amp > maxamp)
			amp = maxamp;
	}
	if (adr)
	{
		if (delay > maxDelay)
		{
			if (stretch <= 0.f)
				stretch = tempoChange;
		}
		else
		if (delay < minDelay)
		{
			if (stretch >= 0.f)
				stretch = -tempoChange;
		}
		else
		{
			stretch = 0.f;
		}
		pSoundTouch->setTempoChange(stretch);
		
		for (i = 0; i < frames; i ++)
			sampleBuffer[i] = (short) buffer[i];
		pSoundTouch->putSamples(sampleBuffer, frames);
		while ((nSamples = pSoundTouch->receiveSamples(sampleBuffer, frames)) > 0)
		{
			for (i = 0; i < nSamples; i ++)
				buffer[i] = (float) sampleBuffer[i];
			play(buffer, nSamples);
		}
	}
	play(buffer, nSamples);
}

void AudioPlayer::ring(short *buffer, int frames)
{
	float floatBuffer[frames];
	for(i=0;i<frames;i++)
	{
		floatBuffer[i] = ((float) buffer[i]) * ringvolume;
	}
	play(floatBuffer, frames);
}


bool AudioPlayer::flush()
{
	if (readyFrames > 0)
		return false;
	else
	{
		switch (driver)
		{
			case PLAY_DRIVER_ALSA:
				if (playback_handle)
				{
					snd_pcm_state_t state = snd_pcm_state(playback_handle);
					if (state == SND_PCM_STATE_RUNNING)
						return false;
				}
				break;
			case PLAY_DRIVER_JACK:
				break;
		}
	}
	return true;
}

void AudioPlayer::end()
{
	working = false;
	timer->stop();
	readyFrames = 0;
	
	if (playback_handle)
	{
		snd_pcm_drop(playback_handle);
		snd_pcm_close(playback_handle);
	}
	playback_handle = NULL;
	
	pSoundTouch->clear();
#ifdef HAVE_LIBJACK
	endJack();
#endif
}

#ifdef HAVE_LIBJACK

void AudioPlayer::initJack()
{
	if ((jack_handle = jack_client_new("drta_player")) == NULL)
		throw Error(tr("JACK: cannot connect to server. (is Jack running?)\n"));
	if (jack_set_process_callback(jack_handle, jack_callback, (void *)this))
		throw Error(tr("JACK: cannot set process callback."));
	jack_out = jack_port_register(jack_handle, "Output", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	if (jack_out == NULL)
		throw Error(tr("JACK: cannot register Output port"));
	if (jack_activate(jack_handle))
		throw Error(tr("JACK: cannot activate client"));
	if (jack_connect(jack_handle, "drta_player:Output", "alsa_pcm:playback_1"))
		emit warning("Couldn't connect DRTA player with Jack port: alsa_pcm:playback_1");
}
 
void AudioPlayer::endJack()
{
	if (jack_handle)
	{
		if (jack_deactivate(jack_handle) == 0)
			jack_client_close(jack_handle);
	}
	jack_handle = NULL;
}

int AudioPlayer::jack_callback(jack_nframes_t nframes, void *arg)
{
	AudioPlayer *p = (AudioPlayer *)arg;
	return (p->jack_play(nframes));
}

int AudioPlayer::jack_play(jack_nframes_t frames)
{
	int fr = (int) frames;
	if (readyFrames > prebuffer)
	{
		if (readyFrames < fr)
		{
			prebuffer = preframes;
		}
		else
		{
			jack_default_audio_sample_t *buf;
			buf = (jack_default_audio_sample_t *)jack_port_get_buffer(jack_out, frames);
			memcpy(buf, jackdata, sizeof(jack_default_audio_sample_t) * fr);
			update_frames(fr);
			prebuffer=0;
		}
	}
	return 0;
}

#endif
