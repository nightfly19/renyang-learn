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
#include "Config.h"
#include "FilePlayer.hpp"
#include "PacketHandler.h"
#include "ring_sounds.h"

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <qobject.h>
#include <qstring.h>
#include <qthread.h>

#define MAXBUFSIZE 65536
#define IN_BUFSIZE 32768

#define TICKTIME 300
#define FILE_TICKTIME 19

FilePlayer::FilePlayer()
{
	setName("FilePlayer");

	player = new Player();
	rsa = new Rsa(RSA_STRENGTH);
	ihu2spx = new Ihu2Spx();

	working = false;
	fast = false;
	spx = false;

	if ((inputBuffer = (char *)malloc(IN_BUFSIZE))==NULL)
		throw Error(Error::IHU_ERR_MEMORY);
	if ((streamBuffer = (char *)malloc(MAXBUFSIZE))==NULL)
		throw Error(Error::IHU_ERR_MEMORY);
	if ((outBuffer = (float *) malloc(MAXBUFSIZE*sizeof(float)))==NULL)
		throw Error(Error::IHU_ERR_MEMORY);

	inFile = NULL;

	player->setup(IHU_DEFAULT_INTERFACE, 0);

	state = NULL;
	speex_bits_init(&bits);
	state = speex_decoder_init(&speex_uwb_mode);
	int enh = 1;
	speex_decoder_ctl(state, SPEEX_SET_ENH, &enh);
	speex_decoder_ctl(state, SPEEX_GET_FRAME_SIZE, &frame_size);
	speex_decoder_ctl(state, SPEEX_GET_SAMPLING_RATE, &rate);

	ring_buffer = (short *) ring_32;
	ring_size = SIZE_RING_32;

	blowfish = NULL;

	timer = new QTimer(this);
	fileTimer = new QTimer(this);

	reset();
	resetStream();

	connect(timer, SIGNAL(timeout()), this, SLOT(checkPlayer()));
	connect(fileTimer, SIGNAL(timeout()), this, SLOT(playCallback()));
}

FilePlayer::~FilePlayer(void)
{
	if (inputBuffer)
		free(inputBuffer);
	if (streamBuffer)
		free(streamBuffer);
	if (outBuffer)
		free(outBuffer);
	if (state)
		speex_decoder_destroy(state);
	speex_bits_destroy(&bits);
	delete player;
	delete rsa;
	delete ihu2spx;
}

void FilePlayer::reset()
{
	nodecrypt = false;
	fast = false;
	total = bytes = 0;
}

void FilePlayer::resetStream()
{
	streamLen = 0;
	streamPtr = streamBuffer;
	sync = STREAM_READ_DATA;
}

void FilePlayer::moveStream()
{
	memmove(streamBuffer, streamPtr, streamLen);
	streamPtr = streamBuffer;
}

void FilePlayer::end()
{
	timer->stop();
	stop();
	stopFile();
	if (spx)
		ihu2spx->end();
	spx = false;
	player->end();
	disableDecrypt();
	callerName = QString::null;
}

void FilePlayer::putData(char *buffer, int len)
{
	if (streamLen > (MAXBUFSIZE - len))
	{
		qWarning("Warning: receiver buffer overloaded.");
		len = MAXBUFSIZE - streamLen;
	}
	memcpy(streamBuffer + streamLen, buffer, len);
	streamLen += len;
}

void FilePlayer::checkPlayer()
{
	if (player->flush())
	{
		timer->stop();
		emitSignal(SIGNAL_FINISH);
	}
}

void FilePlayer::processData()
{
	while (working && (sync != STREAM_DONE) && (sync != STREAM_PLAYER_NOT_READY))
	{
		switch (sync)
		{
			case STREAM_DONE:
			case STREAM_OK:
				if (streamLen < (HEADER_SYNC_LEN + 1))
				{
					sync = STREAM_MISSING_DATA;
					break;
				}
				if (strncmp(streamPtr, HEADER_SYNC_STRING, HEADER_SYNC_LEN) != 0)
				{
					sync = STREAM_OUT_OF_SYNC;
#ifdef IHU_DEBUG
					fprintf(stderr, "OUT OF SYNC (Dump: 0x");
					for(int i=0;i<4;i++)
						fprintf(stderr, "%02x", (unsigned char) streamPtr[i]);
					fprintf(stderr, "\n");
#endif
					break;
				}
				else
				{
					unsigned char packetlen = (unsigned char) streamPtr[HEADER_SYNC_LEN];
					int plen = (int) packetlen;
					if (plen > streamLen)
					{
						sync = STREAM_MISSING_DATA;
						break;
					}
					else
					{
						try {
							if (plen >= MIN_PACKET_SIZE)
							{
								Packet *p = new Packet(plen);
								PacketHandler::readPacket(p, streamPtr, plen);
								processPacket(p);
								delete p;
							}
							else
							{
								sync = STREAM_OUT_OF_SYNC;
								break;
							}
							if (sync != STREAM_PLAYER_NOT_READY)
							{
								if (plen < streamLen)
								{
									streamPtr = streamPtr + plen;
									streamLen = streamLen - plen;
									bytes += plen;
									if (!fast)
										sync = STREAM_DONE;
								}
								else 
								{
									resetStream();
								}
							}
						} catch (Error e)
						{
							emitError(e.getText());
						}
					}
				}
				break;
			case STREAM_OUT_OF_SYNC:
				if (streamLen < 3)
					sync = STREAM_MISSING_DATA;
				else
				{
					do
					{
						streamPtr++;
						streamLen--;
						bytes++;
						if (streamLen <= 0)
						{
							resetStream();
							break;
						}
						if (strncmp(streamPtr, HEADER_SYNC_STRING, HEADER_SYNC_LEN)==0)
						{
							sync = STREAM_OK;
						}
					} while(sync == STREAM_OUT_OF_SYNC);
				}
				break;
			case STREAM_MISSING_DATA:
				sync = STREAM_READ_DATA;
			case STREAM_READ_DATA:
				moveStream();
				readFile();
				if (fast)
					sync = STREAM_DONE;
				break;
			case STREAM_PLAYER_NOT_READY:
				moveStream();
				break;
		}
	}
}

bool FilePlayer::processPacket(Packet *p)
{
	switch (p->getInfo())
	{
		case IHU_INFO_CRYPTED_AUDIO:
			if (blowfish)
				p->decrypt(blowfish);
			else
			{
				if (!nodecrypt)
					emitSignal(SIGNAL_KEYREQUEST);
				else
					fast = true;
				break;
			}
		case IHU_INFO_AUDIO:
			if (p->getDataLen() > MIN_DATA_SIZE)
			{
				if (spx)
				{
					ihu2spx->process(p->getData(), p->getDataLen());
				}
				else
				{
					if (player->ready())
					{
						playData(p->getData(), p->getDataLen());
					}
					else
					{
						sync = STREAM_PLAYER_NOT_READY;
						fast = false;
					}
				}
			}
			break;
		case IHU_INFO_RING:
			if (p->getDataLen() > MIN_DATA_SIZE)
				callerName = p->getData();
			break;
		case IHU_INFO_ANSWER:
		case IHU_INFO_RING_REPLY:
			if (p->getDataLen() > MIN_DATA_SIZE)
				callerName = p->getData();
			break;
		case IHU_INFO_ERROR:
		case IHU_INFO_REFUSE:
		case IHU_INFO_CLOSE:
			break;
		case IHU_INFO_INIT:
			if (!spx)
				player->playShort((short *)ring_32, frame_size);
		case IHU_INFO_RESET:
			disableDecrypt();
			break;
	}
	return true;
}

void FilePlayer::playData(char *buf, int len)
{
	if (state)
	{
		speex_bits_read_from(&bits, buf, len);
		if (speex_decode(state, &bits, outBuffer) < 0)
		{
			emit warning("Warning: wrong decryption key or stream corrupted!");
			disableDecrypt();
		}
		else
		{
			player->put(outBuffer, frame_size);
		}
	}
}

void FilePlayer::emitError(QString text)
{
	emit error(text);
}

void FilePlayer::enableDecrypt(char *passwd, int len)
{
	disableDecrypt();
	blowfish = new Blowfish(passwd, len);
}

void FilePlayer::disableDecrypt()
{
	if (blowfish)
		delete blowfish;
	blowfish = NULL;
}

void FilePlayer::openFile(QString file)
{
	struct stat info;
	inFile = fopen(file.ascii(), "rb");
	if (inFile==NULL)
		throw Error(tr("Can't open ") + file.ascii() + tr("\n") + strerror(errno));
	if (fstat(fileno(inFile), &info)==-1)
		throw Error(tr("Can't stat file - ") + strerror(errno));
	resetStream();
	reset();
	total = (long) info.st_size;
	player->setAdr(false, 0.f, 0.f, 0.f);
	go();
	sync = STREAM_READ_DATA;
	player->start(rate, frame_size);
	fileTimer->start(FILE_TICKTIME, false);
}

void FilePlayer::convertFile(QString file)
{
	spx = true;
	ihu2spx->setup(file, &speex_uwb_mode, rate, frame_size);
	openFile(file);
	fast = true;
}

void FilePlayer::playFile(QString file)
{
	openFile(file);
}

void FilePlayer::readFile()
{
	int n = fread(inputBuffer, 1, IN_BUFSIZE, inFile);
	if (n > 0)
	{
		putData(inputBuffer, n);
		sync = STREAM_OK;
	}
	else
	{
		fileTimer->stop();
		if (spx)
			ihu2spx->end();
		flush();
	}
}

void FilePlayer::seekFile(int off)
{
	if (inFile)
	{
		float offset = (float) off;
		offset /= 100.f;
		offset *= total;
		bytes = (long) offset;
		player->reset();
		resetStream();
		if (fseek(inFile, bytes, SEEK_SET) < 0)
			emitError("can't seek in file!");
	}
}

void FilePlayer::stopFile()
{
	fileTimer->stop();
	if (inFile)
		fclose(inFile);
	inFile = NULL;
	ihu2spx->close();
}

void FilePlayer::stop()
{
	working = false;
}

void FilePlayer::go()
{
	working = true;
}

void FilePlayer::pause()
{
	working = false;
	player->pause(true);
}

void FilePlayer::resume()
{
	working = true;
	player->pause(false);
	processData();
}

long FilePlayer::getBytes()
{
	long temp = bytes;
	bytes = 0;
	return temp;
}

long FilePlayer::getTotal()
{
	return total;
}

QString FilePlayer::getCallerName()
{
	QString name = callerName;
	if (name.isEmpty())
		name = "anonymous";
	return name;
}

void FilePlayer::noDecrypt()
{
	nodecrypt = true;
}

void FilePlayer::emitSignal(signal_type type)
{
	switch(type)
	{
		case SIGNAL_FINISH:
			emit finish();
			break;
		if (working)
		{
			case SIGNAL_KEYREQUEST:
				emit keyRequest();
				break;
		}
		default:
			break;
	}
}

void FilePlayer::flush()
{
	stop();
	timer->start(TICKTIME, false);
}

int FilePlayer::getProgress()
{
	int progress = 0;
	if (total > 0.0)
	{
		float pr = (float) bytes;
		pr /= total;
		pr *= 100.f;
		progress = (int) pr;
	}
	return progress;
}

void FilePlayer::playCallback()
{
	switch(sync)
	{
		case STREAM_DONE:
		case STREAM_PLAYER_NOT_READY:
			sync = STREAM_OK;
			break;
		default:
			break;
	}
	processData();
	player->callback();
}

void FilePlayer::setupPlayer(QString interface)
{
	player->setup(interface, 0);
}
