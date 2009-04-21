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

#include "Error.h"
#include "Player.hpp"

Player::Player()
{
	playback_handle = NULL;
	adr = false;
	ringing = false;
	working = false;
	readyFrames = 0;
	frame_size = 0;
	delay = 0.0;
	pSoundTouch.setChannels(1);
	pSoundTouch.setSetting(SETTING_USE_QUICKSEEK, 1);
	pSoundTouch.setSetting(SETTING_USE_AA_FILTER, 0);
	if ((shortBuffer = (short *) malloc(SAMPLEBUFSIZE*sizeof(short)))==NULL)
		throw Error(Error::IHU_ERR_MEMORY);
}

Player::~Player(void)
{
	end();
	if (shortBuffer)
		free(shortBuffer);
}

void Player::setup(QString interf, int pre)
{
	interface = interf;
	prepackets = pre;
	preframes = prepackets*frame_size;
}

bool Player::isWorking()
{
	return working;
}

float Player::getDelay()
{
	return delay;
}

void Player::start(unsigned int rate, int fsize)
{
	sample_rate = rate;
	frame_size = fsize;
	readyFrames = 0;
	preframes = prepackets*frame_size;
	prebuffer = preframes;
	
	initAlsa(interface.ascii());

	pSoundTouch.clear();
	pSoundTouch.setSampleRate(rate);

	delay = 0.f;
	ringing = false;	
	working = true;
}

void Player::reset()
{
	readyFrames = 0;
	if (playback_handle)
	{
		snd_pcm_drop(playback_handle);
		snd_pcm_prepare (playback_handle);
	}
}

void Player::initAlsa(const char *interf)
{
	snd_pcm_hw_params_t *hw_params;
	
	if ((err = snd_pcm_open (&playback_handle, interf, SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK)) < 0)
		throw Error(tr(QString("ALSA Output: cannot open audio device for playback (%1)").arg(snd_strerror(err))));
	
	if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0)
		throw Error(tr(QString("ALSA Output: cannot allocate hardware parameter structure (%1)").arg(snd_strerror(err))));
	
	if ((err = snd_pcm_hw_params_any (playback_handle, hw_params)) < 0)
		throw Error(tr(QString("ALSA Output: cannot initialize hardware parameter structure (%1)").arg(snd_strerror(err))));

	if ((err = snd_pcm_hw_params_set_access (playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
		throw Error(tr(QString("ALSA Output: cannot set access type (%1)").arg(snd_strerror(err))));

	if ((err = snd_pcm_hw_params_set_format (playback_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0)
		throw Error(tr(QString("ALSA Output: cannot set sample format (%1)").arg(snd_strerror(err))));
	
	if ((err = snd_pcm_hw_params_set_channels (playback_handle, hw_params, 1)) < 0)
		throw Error(tr(QString("ALSA Output: cannot set channel count (%1)").arg(snd_strerror(err))));
	
	if ((err = snd_pcm_hw_params_set_rate (playback_handle, hw_params, sample_rate, 0)) < 0)
		throw Error(tr(QString("ALSA Output: cannot set sample rate (%1)").arg(snd_strerror(err))));

	if ((err = snd_pcm_hw_params (playback_handle, hw_params)) < 0)
		throw Error(tr(QString("ALSA Output: cannot set hw parameters (%1)").arg(snd_strerror(err))));

	snd_pcm_hw_params_free (hw_params);
	
	snd_pcm_sw_params_t *sw_params;
	if ((err = snd_pcm_sw_params_malloc (&sw_params)) < 0)
		throw Error(tr(QString("ALSA Output: cannot allocate hardware parameter structure (%1)").arg(snd_strerror(err))));
				
	if ((err = snd_pcm_sw_params_current (playback_handle, sw_params)) < 0)
		throw Error(tr(QString("ALSA Output: cannot initialize hardware parameter structure (%1)").arg(snd_strerror(err))));

	if ((err = snd_pcm_sw_params_set_avail_min(playback_handle, sw_params, frame_size)) < 0)
		throw Error(tr(QString("ALSA Output: cannot set access type (%1)").arg(snd_strerror(err))));
	
	if ((err = snd_pcm_sw_params (playback_handle, sw_params)) < 0)
		throw Error(tr(QString("ALSA Output: cannot set parameters (%1)").arg(snd_strerror(err))));
	
	if ((err = snd_pcm_prepare (playback_handle)) < 0)
		throw Error(tr(QString("ALSA Output: cannot prepare audio interface for use (%1)").arg(snd_strerror(err))));
}

void Player::setAdr(bool on, float min, float max, float stch)
{
	adr = on;
	if (adr)
	{
		minDelay = min;
		maxDelay = max;
		tempoChange = stch;
		stretch = 0.f;
		pSoundTouch.clear();
	}
}

void Player::playShort(short *buffer, int frames)
{
	if (ready(frames))
	{
		for (i=0; i<frames; i++)
			shortBuffer[readyFrames+i] = buffer[i];
		readyFrames += frames;
	}
}

void Player::play(float *buffer, int frames)
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
		for (i=0; i<frames; i++)
			shortBuffer[readyFrames+i] = (short) buffer[i];
		readyFrames += frames;
	}
}

void Player::update_frames(int frames)
{
	readyFrames -= frames;
	if (readyFrames < 0)
		readyFrames = 0;
	if (readyFrames > 0)
	{
		memcpy(shortBuffer, shortBuffer+frames, sizeof(short)*readyFrames);
	}
}

bool Player::ready(int frames)
{
	int remaining = SAMPLEBUFSIZE - readyFrames;
	if (remaining < frames)
	{
		return false;
	}
	return true;
}

bool Player::ready()
{
	return ready(SAMPLEBUFSIZE>>4);
}

void Player::callback()
{
	state = snd_pcm_state(playback_handle);
	switch(state)
	{
		case SND_PCM_STATE_XRUN:
			if (prebuffer == 0)
				prebuffer = preframes;
			delay = 0.f;
			snd_pcm_prepare (playback_handle);
#ifdef IHU_DEBUG
			qWarning("ALSA: PLAYER XRUN!");
#endif
			break;
		case SND_PCM_STATE_RUNNING:
			if (snd_pcm_delay(playback_handle, &delayNow) == 0)
			{
				fdel = (float) delayNow;
				delay = 0.99 * delay + 0.01 * (fdel/sample_rate);
			}
		case SND_PCM_STATE_PREPARED:
			if (readyFrames > prebuffer)
			{
				if (state == SND_PCM_STATE_PREPARED)
				{
					delay = (minDelay+maxDelay)/2.f;
				}
				err = snd_pcm_writei(playback_handle, shortBuffer, readyFrames);
				if (err > 0)
				{
					update_frames(err);
					prebuffer = 0;
				}
			}
			break;
		default:
			break;
	}
}

void Player::playInit()
{
	prebuffer = frame_size;
}

void Player::put(float *buffer, int frames)
{
	int nSamples = frames;
	if (adr && !ringing)
	{
		if (delay > maxDelay)
		{
			if (stretch <= 0.f)
			{
				stretch = tempoChange;
				pSoundTouch.setTempoChange(stretch);
#ifdef IHU_DEBUG
				qWarning("delay > maxDelay");
#endif
			}
		}
		else
		if (delay < minDelay)
		{
			if (stretch >= 0.f)
			{
				stretch = -tempoChange;
				pSoundTouch.setTempoChange(stretch);
#ifdef IHU_DEBUG
				qWarning("delay < minDelay");
#endif
			}
		}
		else
		{
			if (stretch != 0.f)
			{
				stretch = 0.f;
				pSoundTouch.setTempoChange(stretch);
#ifdef IHU_DEBUG
				qWarning("delay OK");
#endif
			}
		}
		
		for (i = 0; i < frames; i ++)
			sampleBuffer[i] = (short) buffer[i];
		pSoundTouch.putSamples(sampleBuffer, frames);
		while ((nSamples = pSoundTouch.receiveSamples(sampleBuffer, frames)) > 0)
		{
			for (i = 0; i < nSamples; i ++)
				buffer[i] = (float) sampleBuffer[i];
			play(buffer, nSamples);
		}
	}
	else
		play(buffer, nSamples);
}

void Player::pause(bool on)
{
	int enable = 0;
	if (on)
		enable = 1;
	if (playback_handle)
	{
		snd_pcm_pause(playback_handle, enable);
	}
}

bool Player::flush()
{
	if (readyFrames > 0)
	{
		prebuffer = 0;
		return false;
	}
	else
	{
		if (playback_handle)
		{
			snd_pcm_state_t state = snd_pcm_state(playback_handle);
			switch (state)
			{
				case SND_PCM_STATE_RUNNING:
					return false;
				default:
					break;
			}
		}
	}
	return true;
}

void Player::end()
{
	working = false;
	readyFrames = 0;
	
	if (playback_handle)
	{
		snd_pcm_drop(playback_handle);
		snd_pcm_close(playback_handle);
	}
	playback_handle = NULL;
	
	pSoundTouch.clear();
	delay = 0.f;
}

void Player::disableADR(bool on)
{
	ringing = on;
}
