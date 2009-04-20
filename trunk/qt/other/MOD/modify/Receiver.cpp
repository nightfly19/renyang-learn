

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


/* global headers */
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/* local headers */
#include "Receiver.h"
#include "Error.h"
#include "PhoneSound.h"
#include "PacketHandler.h"


#include "TcpServer.h"
#include "SocketSctpServer.h"
#include "SctpSocketHandler.h"

#include <netinet/sctp.h>

#define MAXBUFSIZE 6553600
#define IN_BUFSIZE 16384

#define TICKTIME 100
#define CHECK_TICKTIME 1000
#define FILE_TICKTIME 100
#define FILE_TICKTIME_FAST 20

#define READ_SIZE 2048
#define RING_NUMBER 15

Receiver::Receiver(AudioPlayer *pl, Rsa *r)
	: player(pl), rsa(r)
/*Receiver::Receiver(AudioPlayer *pl)
	: player(pl)*/
{
	
	setName("Receiver");
	s = -1;
	notifier = NULL;
	streamFile = NULL;
	inFile = NULL;
	state = NULL;
	tcpserver = NULL;
	sctpserver = NULL;
	fromFile = false;
	working = false;
	listening = false;
	status = RECEIVER_STATUS_NORMAL;
	rate = 0;
	calls = 0;
	connects = 0;
	reset();
	resetStream();
	speex_bits_init(&bits);
	if ((inputBuffer = (char *)malloc(IN_BUFSIZE))==NULL)
		throw Error(Error::DRTA_ERR_MEMORY);
	if ((streamBuffer = (char *)malloc(MAXBUFSIZE))==NULL)
		throw Error(Error::DRTA_ERR_MEMORY);
	if ((outBuffer = (float *) malloc(MAXBUFSIZE*sizeof(float)))==NULL)
		throw Error(Error::DRTA_ERR_MEMORY);
	calen = sizeof(ca);
	blowfish = NULL;
	timer = new QTimer(this);
	fileTimer = new QTimer(this);
	checkTimer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(checkAudioPlayer()));
	connect(fileTimer, SIGNAL(timeout()), this, SLOT(readFile()));
	connect(checkTimer, SIGNAL(timeout()), this, SLOT(checkConnection()));
}

Receiver::~Receiver(void)
{
	if (inputBuffer)
		free(inputBuffer);
	if (streamBuffer)
		free(streamBuffer);
	if (outBuffer)
		free(outBuffer);
	if (state)
		speex_decoder_destroy(state);
	if (streamFile)
		fclose(streamFile);
	speex_bits_destroy(&bits);
}

void Receiver::changeMode(char mode)
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

	initAudioPlayer(status);

	playing = true;
    }
    catch (Error e)
    {
	emitError(e.getText());
    }
}

void Receiver::initAudioPlayer(receiver_status newstatus)
{
	switch(newstatus)
	{
		case RECEIVER_STATUS_MUTE:
			break;
		default:
			player->init(rate, frame_size);
			break;
	}
}

void Receiver::reset()
{
	refuse = false;
	reply = false;
	newconnected = true;
	halfconnected = false;
	connected = false;
	realtime = true;
	flushing = false;
	nodecrypt = false;
	playing = false;
	total = bytes = packets = 0;
	speexmode = 0x0;
}

void Receiver::resetStream()
{
	streamLen = 0;
	streamPtr = streamBuffer;
	sync = STREAM_READ_DATA;
}

void Receiver::dump(QString file)
{
	if (!file.isEmpty())
	{
		streamFile = fopen(file.ascii(), "ab");
		if (streamFile==NULL)
			throw Error(tr("Can't open ") + file.ascii() + tr("\n") + strerror(errno));
	}
	else
	{
		if (streamFile)
		{
			fclose(streamFile);
			streamFile = NULL;
		}
	}
}

void Receiver::changeStatus(receiver_status newstatus)
{
	switch(newstatus)
	{
		case RECEIVER_STATUS_MUTE:
			player->end();
			break;
		default:
			if (working && playing && (!player->isWorking()))
				initAudioPlayer(newstatus);
			break;
	}
	status = newstatus;
}

void Receiver::start(int socket, int pt)
{
	s = socket;
	protocol = pt;
	::getpeername(s, (struct sockaddr *)&ca, &calen);
	resetStream();
	reset();
	go();

	notifier = new QSocketNotifier(s, QSocketNotifier::Read, this);
	connect(notifier,SIGNAL(activated(int)),this, SLOT(receive()));
}

void Receiver::Listen(int port, bool udp, bool tcp , bool sctp , bool sctp_udp)
{
	int sd;
	listening = true;
	if (udp) {
		if ((sd = ::socket(AF_INET, SOCK_DGRAM, 0)) == -1)
			throw Error(tr("Can't initalize socket! (socket())"));
		if (fcntl(sd, F_SETFL, O_NONBLOCK) == -1)
			throw Error(tr("fcntl can't set socket property\n") + strerror(errno));
	
		memset(&sa, 0, sizeof(sa));
		sa.sin_family = AF_INET;
		sa.sin_port = htons(port);
		sa.sin_addr.s_addr = htonl(INADDR_ANY);
	
		if (bind(sd, (struct sockaddr *) &sa, sizeof(sa)) == -1)
			throw Error(tr(QString("can't listen on port %1 (%2)").arg(port).arg(strerror(errno))));
		
		start(sd, DRTA_UDP);
	} else if (tcp) {
		tcpserver = new TcpServer(this, port);
		connect( tcpserver, SIGNAL(newConnectTCP(int)), this, SLOT(newConnectionTCP(int)));
	} else if (sctp) {
		sctpserver = new SocketSctpServer(this, port); 
		connect( sctpserver, SIGNAL(newConnectSCTP(int)), this, SLOT(newConnectionSCTP(int)));

		sctpserver -> SctpEnable();
		sctpserver -> SctpSetMaxStream(5);
		SctpSocketHandler::SctpSetNoDelay( sctpserver -> getSocketFd() );
		SctpSocketHandler::SctpTurnOnAllEvent( sctpserver -> getSocketFd() );


		sctpserver -> Bind_Listen();
	}else if(sctp_udp){
		debug("try to bind sctp (udp style)");
		if ((sd = ::socket(AF_INET,  SOCK_SEQPACKET, IPPROTO_SCTP )) == -1)
			throw Error(tr("Can't initalize socket! (socket())"));

			/*
		if (fcntl(sd, F_SETFL, O_NONBLOCK) == -1)
			throw Error(tr("fcntl can't set socket property\n") + strerror(errno));
			*/
	
		memset(&sa, 0, sizeof(sa));
		sa.sin_family = AF_INET;
		sa.sin_port = htons(port);
		sa.sin_addr.s_addr = htonl(INADDR_ANY);
	
		if (::bind(sd, (struct sockaddr *) &sa, sizeof(sa)) == -1)
			throw Error(tr(QString("can't listen on port %1 (%2)").arg(port).arg(strerror(errno))));
		if (::listen(sd, 1) < 0 ){
			throw Error(tr(QString("can't listen on port %1 (%2)").arg(port).arg(strerror(errno))));
		}
		
		start(sd, DRTA_SCTP_UDP);
	}
	waitConnection();
}

void Receiver::close()
{
	if (notifier)
		delete notifier;
	notifier = NULL;
	
	if(s!=-1)
		::close(s);
	s = -1;
	
	if (tcpserver)
		delete tcpserver;
	tcpserver = NULL;

	if (sctpserver)
		delete sctpserver;
	sctpserver = NULL;
}

void Receiver::end()
{
	close();
	
	checkTimer->stop();
	timer->stop();
	
	stop();
	listening = false;
	
	stopFile();
	fromFile = false;
	
	player->end();
	
	disableDecrypt();
	
	callerName = QString::null;
}

void Receiver::emitSctpEvent(void * inmessage)
{
	//int error;
	union sctp_notification* sctp_no = (union sctp_notification*) inmessage;

	QString s;
	int assid=0;


	// TODO: don't need check anymore?
	if (true){
	//if( sctp_no -> sn_header. sn_flags & MSG_NOTIFICATION){ // sctp event notify

		switch (sctp_no->sn_header.sn_type){ 
			case SCTP_ASSOC_CHANGE:
				switch(sctp_no->sn_assoc_change.sac_state)
				{
					assid = sctp_no->sn_assoc_change.sac_assoc_id;
					s = QString::number(assid);
					case SCTP_COMM_UP:
						emit sigSctpEvent("ASSOC CHANGE: Recieved SCTP_COMM_UP - assoc_id="+s);
						break;
					case SCTP_COMM_LOST:
						emit sigSctpEvent("ASSOC CHANGE: Recieved SCTP_COMM_LOST - assoc_id="+s);
						break;
					case SCTP_RESTART:
						emit sigSctpEvent("ASSOC CHANGE: Recieved SCTP_RESTART - assoc_id="+s);
						break;
					case SCTP_SHUTDOWN_COMP:
						emit sigSctpEvent("ASSOC CHANGE: Recieved SCTP_SHUTDOWN_COMP - assoc_id="+s);
						break;
					case SCTP_CANT_STR_ASSOC:
						emit sigSctpEvent("ASSOC CHANGE: Recieved SCTP_CANT_STR_ASSOC - assoc_id="+s);
						break;
				}
				break;
			case SCTP_PEER_ADDR_CHANGE:
				emit sigSctpEvent("PEER ASSOC CHANGE");
				break;
			case SCTP_SEND_FAILED:
				/* TODO: partial reliable */

				emit sigPrSendFailed();
				emit sigSctpEvent("SEND FAILED");
				break;
			case SCTP_REMOTE_ERROR:
				emit sigSctpEvent("REMOTE ERROR");
				break;
			case SCTP_SHUTDOWN_EVENT:
				emit sigSctpEvent("SHUTDOWN EVENT");
				break;
			case SCTP_PARTIAL_DELIVERY_EVENT:
				emit sigSctpEvent("PARTIAL DELIVERY EVENT");
				break;
			case SCTP_ADAPTATION_INDICATION: 
				assid = sctp_no->sn_shutdown_event.sse_assoc_id;
				s = QString::number(assid);
				emit sigSctpEvent(tr("ADAPTION INDICATION assoc_id = ") + s);
				break;
		}; 
	}else{
		debug(" not sctp notification @@");
	}
}

void Receiver::receive()
{
	struct sockaddr_in sctp_from;
	socklen_t sctp_fromlen = sizeof( sctp_from );
	struct sctp_sndrcvinfo sndrcvinfo;
	QString ip_from;
	int msg_flag = 0;

	bool valid_data  = true; // for verify sctp notification

	if (working)
	{
		int rlen = 0;
		
		switch(protocol)
		{
			case DRTA_UDP:
				rlen = recvfrom(s, inputBuffer, IN_BUFSIZE, 0, (struct sockaddr *)&ca, &calen);
				break;
			case DRTA_TCP:
				rlen = recv(s, inputBuffer, IN_BUFSIZE, 0);
				break;
			case DRTA_SCTP:
			case DRTA_SCTP_UDP:
				rlen = sctp_recvmsg(s, inputBuffer, IN_BUFSIZE
					,(sockaddr*) &sctp_from, &sctp_fromlen , &sndrcvinfo , &msg_flag);

				if( msg_flag & MSG_NOTIFICATION){ // sctp event notify
					//emit sigSctpEvent(s);
					emitSctpEvent(inputBuffer );
					//debug("sctp recv -> msg_flag = %d" , msg_flag);

					valid_data  = false;
				}else{
					ip_from = inet_ntoa( (sctp_from.sin_addr) );
					emit sigRecvFrom( ip_from );	
				}

				break;
		}

		if (rlen > 0)
		{
			if( valid_data ){
				bytes += rlen;
				total += rlen;
				ledOn(true);
				putData(inputBuffer, rlen);
			}
		}
		else
		{
			flush();
		}
	}
}

void Receiver::putData(char *buffer, int len)
{
	if (streamFile && !fromFile)
	{
		fwrite(buffer, 1, len, streamFile);
		fflush(streamFile);
	}
	
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

void Receiver::checkAudioPlayer()
{
	if (flushing)
	{
		if (player->flush())
			emitSignal(SIGNAL_FINISH);
	}
	else
	{
		switch(sync)
		{
			case STREAM_PLAYER_NOT_READY:
				sync = STREAM_OK;
				processData();
				break;
			case STREAM_READ_DATA:
				timer->stop();
				realtime = true;
				break;
			default:
				break;
		}
	}
}

void Receiver::processData()
{
	while ((sync != STREAM_READ_DATA) && (sync != STREAM_PLAYER_NOT_READY))
	{
		switch (sync)
		{
			case STREAM_OK:
				if (streamLen < (HEADER_SYNC_LEN + 1))
				{
					sync = STREAM_MISSING_DATA;
					break;
				}
			case STREAM_DATA:
				if (strncmp(streamPtr, HEADER_SYNC_STRING, HEADER_SYNC_LEN) != 0)
				{
					sync = STREAM_OUT_OF_SYNC;
#ifdef DRTA_DEBUG
					fprintf(stderr, "OUT OF SYNC (packet # %d. Dump: 0x", packets);
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
							Packet *p = new Packet(plen);
							PacketHandler::readPacket(p, streamPtr, plen);
							processPacket(p);
							if (sync != STREAM_PLAYER_NOT_READY)
							{
								if (plen < streamLen)
								{
									streamPtr = streamPtr + plen;
									streamLen = streamLen - plen;
									sync = STREAM_OK;
								}
								else 
								{
									sync = STREAM_READ_DATA;
									resetStream();
								}
							}
							delete p;
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
						if (streamLen <= 0)
						{
							sync = STREAM_READ_DATA;
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
			case STREAM_PLAYER_NOT_READY:
				memmove(streamBuffer, streamPtr, streamLen);
				streamPtr = streamBuffer;
				break;
		}
	}
}

void Receiver::processPacket(Packet *p)
{
	int mode = p->getMode();
	QString buf;
	if (mode)
	{
		packets++; 
		if (mode != speexmode)
			changeMode(mode);
	}
	switch (p->getInfo())
	{
		case DRTA_INFO_CRYPTED_AUDIO:
			if (blowfish)
				p->decrypt(blowfish);
			else
			{
				if (!nodecrypt)
				{
					emitSignal(SIGNAL_KEYREQUEST);
				}
				break;
			}
		case DRTA_INFO_AUDIO:
			switch (status)
			{
				case RECEIVER_STATUS_MUTE:
					break;
				case RECEIVER_STATUS_NORMAL:
					if (player->ready())
					{
						playData(p->getData(), p->getDataLen());
					}
					else
					{
						sync = STREAM_PLAYER_NOT_READY;
						timer->start(TICKTIME, false);
					}
					break;
			}
			break;
		case DRTA_INFO_NEW_KEY:
			if (!fromFile)
			{
				char *out;
				int len = rsa->decrypt(p->getData(), p->getDataLen(), &out);
				if (blowfish)
					delete blowfish;
				blowfish = new Blowfish(out, len);
				emitSignal(SIGNAL_NEWKEY);
				free(out);
			}
			break;
		case DRTA_INFO_KEY_REQUEST:
			if (!fromFile)
			{
				rsa->setPeerPublicKey(p->getData(), p->getDataLen());
				emitSignal(SIGNAL_SENDNEWKEY);
			}
			break;
		case DRTA_INFO_RING:
			if (p->getDataLen() > 0)
				callerName = p->getData();
			if (!fromFile)
			{
				emit warning(QString("!! RING from %1 (%2) !!").arg(getIp()).arg(getCallerName()));
				goRing();
				emitSignal(SIGNAL_RINGREPLY);
			}
			break;
		case DRTA_INFO_ANSWER:
		case DRTA_INFO_RING_REPLY:
			reply = true;
			if (p->getDataLen() > 0)
				callerName = p->getData();
			break;
		case DRTA_INFO_REFUSE:
			refuse = true;
		case DRTA_INFO_CLOSE:
			if (!fromFile)
				flush();
			break;
		case DRTA_INFO_INIT:
			packets = 0;
			ring(frame_size);
		case DRTA_INFO_RESET:
			disableDecrypt();
			break;
		case DRTA_INFO_MSG:
			//debug("RECV MSG : %s" , p->getData());
			buf = QString :: fromUtf8( p->getData() );	
			emit sigRecvMsg( buf );
			break;
	}
}

void Receiver::ring(int size)
{
	switch (status)
	{
		case RECEIVER_STATUS_MUTE:
			break;
		default:
			player->ring(ring_buffer, size);
			break;
	}
}

void Receiver::playData(char *buf, int len)
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

void Receiver::emitError(QString text)
{
	emit error(text);
}

void Receiver::enableDecrypt(char *passwd, int len)
{
	disableDecrypt();
	blowfish = new Blowfish(passwd, len);
}

void Receiver::disableDecrypt()
{
	if (blowfish)
		delete blowfish;
	blowfish = NULL;
}

void Receiver::playFile(QString file)
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
	fromFile = true;
	read_size = READ_SIZE;
	player->setAdr(false, 0.f, 0.f, 0.f);
	go();
	startFile();
}

void Receiver::startFile()
{
	readFile();
	fileTimer->start(FILE_TICKTIME, false);
}

void Receiver::readFile()
{
	if (working)
	{
		if (sync == STREAM_READ_DATA)
		{
			int n = fread(inputBuffer, 1, read_size, inFile);
			if (n > 0)
			{
				ledOn(true);
				putData(inputBuffer, n);
				bytes += n;
				float progress = (float) bytes;
				progress /= total;
				progress *= 100.f;
				emit fileProgress((int) progress);
			}
			else
				flush();
		}
	}
}

void Receiver::seekFile(int off)
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

void Receiver::stopFile()
{
	fileTimer->stop();
	if (inFile)
		fclose(inFile);
	inFile = NULL;
}

void Receiver::waitConnection()
{
	checkTimer->start(CHECK_TICKTIME, false);
}

void Receiver::newConnectionTCP(int socket)
{
	close();
	start(socket, DRTA_TCP);
	halfconnected = true;
	checkConnection();
	ledOn(true);
}
void Receiver::newConnectionSCTP(int socket)
{
	close();
	start(socket, DRTA_SCTP);
	halfconnected = true;
	checkConnection();
	ledOn(true);
}

void Receiver::checkConnection()
{
	if (halfconnected)
		emit warning(QString("Incoming connection from %1!").arg(getIp()));
	if (total > 0)
	{
		if (listening)
		{
			halfconnected = true;
			emitSignal(SIGNAL_RINGREPLY);
			if (packets > 0)
			{
				calls++;
				connected = true;
				emit warning(QString("New call from %1!").arg(getIp()));
			}
		}
		else
		{
			if (packets > 0)
			{
				halfconnected = true;
				connected = true;
				emit warning(QString("Connected with %1 (%2).").arg(getCallerName()).arg(getIp()));
			}
		}
	}
	else
	{
		if (listening)
		{
			if (halfconnected)
				emitSignal(SIGNAL_RINGREPLY);
			else
				emit message(QString("Waiting for calls  -  Missed calls: %1 (%2 connections)").arg(calls).arg(connects));
		}
	}
	if (halfconnected && newconnected)
	{
		newconnected = false;

		if (tcpserver)
			delete tcpserver;
		tcpserver = NULL;

		if (sctpserver)
			delete sctpserver;
		sctpserver = NULL;

		if (listening)
			connects++;
		emit newSocket(s, protocol, ca);
		emitSignal(SIGNAL_RINGREPLY);
	}
	if (connected)
		checkTimer->stop();
}

void Receiver::goRing()
{
	for (int i=0; i<RING_NUMBER; i++)
		ring(ring_size);
}

void Receiver::stop()
{
	working = false;
}

void Receiver::go()
{
	working = true;
}

void Receiver::swap()
{
	working = !working;
}

long Receiver::getBytes()
{
	long temp = bytes;
	bytes = 0;
	ledOn(false);
	return temp;
}

long Receiver::getTotal()
{
	return total;
}

QString Receiver::getIp()
{
	return QString(inet_ntoa(ca.sin_addr));
}

QString Receiver::getCallerName()
{
	QString name = callerName;
	if (name.isEmpty())
		name = "none ";
	return name;
}

int Receiver::getConnections()
{
	return connects;
}

int Receiver::getCalls()
{
	return calls;
}

void Receiver::resetCalls()
{
	calls = 0;
	connects = 0;
}

void Receiver::ledOn(bool on)
{
	if (working)
		emit ledEnable(on);
}

void Receiver::noDecrypt()
{
	nodecrypt = true;
	read_size = IN_BUFSIZE;
	fileTimer->changeInterval(FILE_TICKTIME_FAST);
}

bool Receiver::refused()
{
	return refuse;
}

bool Receiver::replied()
{
	return reply;
}

void Receiver::emitSignal(signal_type type)
{
	switch(type)
	{
		case SIGNAL_FINISH:
			emit finish();
			break;
		if (working)
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
		}
		default:
			break;
	}
}

void Receiver::flush()
{
	stop();
	flushing = true;
	timer->start(TICKTIME, false);

	emitSignal(SIGNAL_FINISH);
	emit sigRemoteTerminate();
}

