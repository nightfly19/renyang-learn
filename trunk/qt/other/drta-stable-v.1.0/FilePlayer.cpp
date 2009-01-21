

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



#include "FilePlayer.h"
#include "PhoneSound.h"
#include "Error.h"


#define MAXBUFSIZE 65536
#define IN_BUFSIZE 16384

#define TICKTIME 100
#define CHECK_TICKTIME 1000
#define FILE_TICKTIME 100
#define FILE_TICKTIME_FAST 20

#define READ_SIZE 2048
#define RING_NUMBER 15

FilePlayer::FilePlayer(Player* p)
	: player(p)
{
	setName("FilePlayer"); //for qobject

	inFile = NULL;
	state = NULL;

	reset();

	status = FP_STATUS_NORMAL;
	rate = 0;


	/* speex initial */
	speex_bits_init(&bits);

	/* initial buffer size */
	if ((inputBuffer = (char *)malloc(IN_BUFSIZE))==NULL)
		throw Error(Error::DRTA_ERR_MEMORY);
	if ((streamBuffer = (char *)malloc(MAXBUFSIZE))==NULL)
		throw Error(Error::DRTA_ERR_MEMORY);
	if ((outBuffer = (float *) malloc(MAXBUFSIZE*sizeof(float)))==NULL)
		throw Error(Error::DRTA_ERR_MEMORY); 

	/* set timers */
	timer = new QTimer(this);
	fileTimer = new QTimer(this);
	checkTimer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(checkPlayer()));
	connect(fileTimer, SIGNAL(timeout()), this, SLOT(readFile()));
	connect(checkTimer, SIGNAL(timeout()), this, SLOT(checkConnection()));
}

FilePlayer::~FilePlayer()
{

	if (inputBuffer)
		free(inputBuffer);
	if (streamBuffer)
		free(streamBuffer);
	if (outBuffer)
		free(outBuffer);


	/* destroy speex's data */
	if (state)
		speex_decoder_destroy(state);
	speex_bits_destroy(&bits);

}

void FilePlayer::reset()
{
	flushing = false;
	speexmode = 0x0;

}

void FilePlayer::initPlayer(fileplayer_status newstatus)
{
	switch(newstatus)
	{
		case FP_STATUS_MUTE:
			break;
		default:
			player->init(rate, frame_size);
			break;
	}
}

void FilePlayer::changeMode(char mode)
{
	try
	{
		player->end();
		speexmode = mode;
		SpeexMode *spmode = NULL;
		switch(mode)
		{
			case DRTA_INFO_MODE_ULTRAWIDE:
				spmode = (SpeexMode *) &speex_uwb_mode;
				ring_buffer = (short *) ring_32;
				ring_size = SIZE_RING_32;
				break;
			case DRTA_INFO_MODE_WIDE:
				spmode = (SpeexMode *) &speex_wb_mode;
				ring_buffer = (short *) ring_16;
				ring_size = SIZE_RING_16;
				break;
			case DRTA_INFO_MODE_NARROW:
				spmode = (SpeexMode *) &speex_nb_mode;
				ring_buffer = (short *) ring_8;
				ring_size = SIZE_RING_8;
				break;
			default:
				throw Error("stream may be corrupted");
		}

		if (state)
			speex_decoder_destroy(state);
		state = speex_decoder_init(spmode);
		int enh = 1;
		speex_decoder_ctl(state, SPEEX_SET_ENH, &enh);
		speex_decoder_ctl(state, SPEEX_GET_FRAME_SIZE, &frame_size);
		speex_decoder_ctl(state, SPEEX_GET_SAMPLING_RATE, &rate);

		initPlayer(status);

		//playing = true; //no this value's
	}
	catch (Error e)
	{
		emit error(e.getText());
	}
}

void FilePlayer::playData(char *buf, int len)
{
	if (state)
	{
		speex_bits_read_from(&bits, buf, len);
		if (speex_decode(state, &bits, outBuffer) < 0)
		{
			emit warning("Warning: wrong decryption key or stream corrupted!");
			//disableDecrypt();
		}
		else
		{
			player->put(outBuffer, frame_size);
		}
	}
}

void FilePlayer::checkPlayer()
{
	if (flushing)
	{
		if (player->flush())
			emitSignal(SIGNAL_FINISH);
	}
	else
	{
	}
}

void FilePlayer::emitSignal(signal_type type)
{
	switch(type)
	{
		case SIGNAL_FINISH:
			emit finish();
			break;
			/*if (working)
			{
				case SIGNAL_RINGREPLY:
					emit ringReply();
					break;
				case SIGNAL_SENDNEWKEY:
					emit sendNewKey();
					break;
				case SIGNAL_KEYREQUEST:
					emit keyRequest();
					break;
				case SIGNAL_NEWKEY:
					QString text = blowfish->isAsciiKey() ? blowfish->getPass() : "random key";
					emit newKey(text);
					break;
			}*/
		default:
			break;
	}
}

/* File Handler */
void FilePlayer::startFile()
{
	readFile();
	fileTimer->start(FILE_TICKTIME, false);
}

void FilePlayer::readFile()
{
	int n = fread(inputBuffer, 1, read_size, inFile);
	if (n > 0)
	{
		//	ledOn(true);
		putData(inputBuffer, n);
		/*bytes += n;
		float progress = (float) bytes;
		progress /= total;
		progress *= 100.f;*/
		//	emit fileProgress((int) progress); // emit nothing by maple
	}
	else
		flush();
}

void FilePlayer::seekFile(int off)
{
	/*
	float offset = (float) off;
	offset /= 100.f;
	offset *= total;
	bytes = (long) offset;
	player->reset();
	resetStream(); // no socket (maple)
	if (fseek(inFile, bytes, SEEK_SET) < 0)
		emitError("can't seek in file!");
		*/
}

void FilePlayer::flush()
{
	//stop(); //don't need to chang working flag (maple)
	flushing = true;
	timer->start(TICKTIME, false);
}

void FilePlayer::putData(char *buffer, int len)
{
	/*
	if (streamFile && !fromFile)
	{
		fwrite(buffer, 1, len, streamFile);
		fflush(streamFile);
	}*/

	if (streamLen > (MAXBUFSIZE - len))
	{
		qWarning("Warning: receiver buffer overloaded.");
		len = MAXBUFSIZE - streamLen;
	}
	memcpy(streamBuffer + streamLen, buffer, len);
	streamLen += len;

	if (realtime)
	{
		if (sync == STREAM_READ_DATA)
		{
			sync = STREAM_OK;
			processData();
		}
		else
		{
			realtime = false;
		}
	}
}
