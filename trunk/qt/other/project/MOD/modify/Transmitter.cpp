

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

#include "Transmitter.h"
#include "Config.h"
#include "Error.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "PacketHandler.h"
#include <netinet/sctp.h>

#include "SocketSctpClient.h"
#include "SctpSocketHandler.h"

#define MAXBUFSIZE 65536
#define RINGTIME 3000

Transmitter::Transmitter(Recorder *rec, Rsa *r)
	: recorder(rec), rsa(r)
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::Transmitter()"));
#endif
	setName("Transmitter");
	s = -1;
	salen = sizeof(sa);
	total = bytes = 0;
	rate = 0;
	outFile = NULL;
	state = NULL;
	blowfish = NULL;
	speexmode = 0;
	_enable_sctp = false;
	ready = 0;
	working = false;
	recording = false;
	ringing = false;
	transmitting = false;
	readBuffer = NULL;

	ttl = 100;

	if ((out = (char *) malloc(MAXBUFSIZE))==NULL)
		throw Error(Error::DRTA_ERR_MEMORY);
	if ((buffer = (float *) malloc(MAXBUFSIZE*sizeof(float)))==NULL)
		throw Error(Error::DRTA_ERR_MEMORY);
	if ((prebuffer = (float *) malloc(REC_BUFSIZE*sizeof(float)))==NULL)
		throw Error(Error::DRTA_ERR_MEMORY);
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(sendRing()));
	connect(recorder, SIGNAL(data(float*, int)), this, SLOT(processData(float*,int)));
	status = TRANSMITTER_STATUS_WAITING;
}

Transmitter::~Transmitter(void)
{
#ifdef REN_DEBUG
	qWarning("Transmitter::~Transmitter()");
#endif
	if (out)
		free(out);
	if (buffer)
		free(buffer);
	if (prebuffer)
		free(prebuffer);
	if (state)
		speex_encoder_destroy(state);
	speex_bits_destroy(&bits);
}

void Transmitter::setup(int srate, int quality, int abr, int vbr, float vbr_quality, int complexity, int vad, int dtx, int txstop)
{
#ifdef REN_DEBUG
	qWarning("Transmitter::setup()");
#endif
	if (recording)
	{
		recorder->end();
		stop();
	}

	SpeexMode *mode = NULL;
	
	switch (srate)
	{
		case 32000:
			mode = (SpeexMode *) &speex_uwb_mode;
			speexmode = DRTA_INFO_MODE_ULTRAWIDE;
			break;
		case 16000:
			mode = (SpeexMode *) &speex_wb_mode;
			speexmode = DRTA_INFO_MODE_WIDE;
			break;
		case 8000:
			mode = (SpeexMode *) &speex_nb_mode;
			speexmode = DRTA_INFO_MODE_NARROW;
			break;
	}
	
	if (state)
		speex_encoder_destroy(state);
	
	state = speex_encoder_init(mode);

	speex_encoder_ctl(state, SPEEX_SET_SAMPLING_RATE, &srate);
	speex_encoder_ctl(state, SPEEX_SET_COMPLEXITY, &complexity);
	if (vbr)
	{
		speex_encoder_ctl(state, SPEEX_SET_VBR, &vbr);
		speex_encoder_ctl(state, SPEEX_SET_VBR_QUALITY, &vbr_quality);
	}
	else
	{
		speex_encoder_ctl(state, SPEEX_SET_QUALITY, &quality);
		speex_encoder_ctl(state, SPEEX_SET_VAD, &vad);
	}
	if (abr)
		speex_encoder_ctl(state, SPEEX_SET_ABR, &abr);
	speex_encoder_ctl(state, SPEEX_SET_DTX, &dtx);
	speex_encoder_ctl(state, SPEEX_GET_FRAME_SIZE, &frame_size);
	
	stoptx = (int) ((srate / frame_size)*txstop);
	
	speex_bits_init(&bits);
	
	rate = srate;

	if (recording)
	{
		initRecorder();
		go();
	}
}

void Transmitter::init(int prot)
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::init(%1)").arg(prot));
#endif
	protocol = prot;
	if(prot == DRTA_SCTP || prot == DRTA_SCTP_UDP){
		_enable_sctp = true;
	}
	
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;

	speak = bytes = total = 0;

	memset(out, 0, MAXBUFSIZE);
	
	memset(prebuffer, 0x0, sizeof(float)*REC_BUFSIZE);
	
}

void Transmitter::dump(QString name, QString file)
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::dump(%1,%2)").arg(name).arg(file));
#endif
	if(name.length() > DRTA_MAX_NAME_LEN)
		name.truncate(DRTA_MAX_NAME_LEN);
	myName = name;

	if (!file.isEmpty())
	{
		outFile = fopen(file.ascii(), "ab");
		if (outFile==NULL)
			throw Error(tr("can't open file - ") + strerror(errno));
	}
	else
	{
		if (outFile)
			fclose(outFile);
		outFile = NULL;
	}
}

void Transmitter::initRecorder()
{
#ifdef REN_DEBUG
	qWarning("Transmitter::initRecorder()");
#endif
	switch (status)
	{
		case TRANSMITTER_STATUS_MUTE:
			break;
		default:
			ready = toRead = 0;
			bufptr = buffer;
			recorder->start(rate);
			break;
	}
}

void Transmitter::go()
{
#ifdef REN_DEBUG
	qWarning("Transmitter::go()");
#endif
	working = true;
}

void Transmitter::stop()
{
#ifdef REN_DEBUG
	qWarning("Transmitter::stop()");
#endif
	working = false;
}

void Transmitter::prepare()
{
#ifdef REN_DEBUG
	qWarning("Transmitter::prepare()");
#endif
	switch(status)
	{
		case TRANSMITTER_STATUS_NORMAL:
			status = TRANSMITTER_STATUS_WAITING;
		default:
			sendInitPacket();
			changeStatus(status);
			break;
	}
	transmitting = true;
}

void Transmitter::changeStatus(transmitter_status newstatus)
{
#ifdef REN_DEBUG
	qWarning("Transmitter::changeStatus()");
#endif
	status = newstatus;
	switch (status)
	{
		case TRANSMITTER_STATUS_MUTE:
			recorder->end();
			break;
		default:
			if (working && recording)
			{
				if (!recorder->isWorking())
					initRecorder();
				emit message("Audio input started.");
			}
			break;
	}
	ledOn(false);
}

/*
*	只會被 DrtaMW2 呼叫....
*	當 Receiver接到newConnection時 傳給  DrtaMW2 然後再 設定Transmitter
*
*/ 
void Transmitter::newConnection(int socket, struct sockaddr_in ca, int prot)
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::newConnection()"));
#endif
	init(prot);
	
	switch (prot)
	{
		case DRTA_UDP:
			if ((::connect(socket, (struct sockaddr *)&ca, salen))==-1)
				throw Error(strerror(errno));
			break;
		case DRTA_SCTP_UDP:
			if ((::connect(socket, (struct sockaddr *)&ca, salen))==-1)
				throw Error(strerror(errno));
			break;
	}
	::getpeername(socket, (struct sockaddr *)&sa, &salen);
	start(socket);
}

void Transmitter::start(int socket)
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::start(%1)").arg(socket));
#endif
	s = socket;
	go();
	recording = true;
}

int Transmitter::call(QString host, int port, int prot)
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::call(%1,%2,%3)").arg(host).arg(port).arg(prot));
#endif
	init(prot);

	int sd = -1;
	
	sa.sin_port = htons(port);
	
	if (inet_aton(host.ascii(), &sa.sin_addr) == 0)
	{
		struct hostent *he;

		he = gethostbyname(host.ascii());
		if (he == NULL)
			throw Error(tr("can't resolve ") + host.ascii());
		sa.sin_addr = *(struct in_addr *) he ->h_addr;
	}
	
	int type = 0;
	
	switch(protocol)
	{
		case DRTA_UDP:
			type = SOCK_DGRAM;
			break;
		case DRTA_TCP:
		case DRTA_SCTP:
			type = SOCK_STREAM;
			break;
		case DRTA_SCTP_UDP:
			type = SOCK_SEQPACKET;
			break;
		default:
			throw Error("unknown protocol");
	}
	
	if ((sd = socket(AF_INET, type, 0)) == -1)
		throw Error(tr("can't initalize socket (") + strerror(errno)+ tr(")"));
	
	if ((::connect(sd, (struct sockaddr *)&sa, sizeof(sa)))==-1)
		throw Error(strerror(errno));
	
	start(sd);
	
	initRecorder();
	
	return sd;
}

void Transmitter::end()
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::end()"));
#endif
	stop();
	
	recording = transmitting = false;
	
	timer->stop();
	
	total = 0;

	recorder->end();
	
	if (s!=-1)
		::close(s);
	s = -1;
	
	if (outFile)
		fclose(outFile);
	outFile = NULL;
}

void Transmitter::enableCrypt(char *passwd, int len)
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::enableCrypt()"));
#endif
	disableCrypt();
	blowfish = new Blowfish(passwd, len);
	sendResetPacket();
}

void Transmitter::disableCrypt()
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::disableCrypt()"));
#endif
	if (blowfish)
		delete blowfish;
	blowfish = NULL;
}

int Transmitter::isSpeaking(float *samples, int sample)
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::isSpeaking()"));
#endif
	int i;
	for (i=0; i<sample; i++)
		if (samples[i] >= threshold)
			return 1;
	return 0;
}

void Transmitter::processData(float *samples, int nsample)
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::processData()"));
#endif
	readBuffer = samples;
	while (working && (nsample > 0))
	{
		toRead = frame_size - ready;
		if (nsample < toRead)
		{
			memcpy(bufptr, readBuffer, nsample*sizeof(float));
			ready += nsample;
			bufptr += nsample;
			nsample = 0;
			break;
		}
		else
		if (nsample >= toRead)
		{
			memcpy(bufptr, readBuffer, toRead*sizeof(float));
			ready += toRead;
			bufptr += toRead;
			readBuffer += toRead;
			nsample -= toRead;
			if (ready == frame_size)
			{
				if (transmitting)
					send(buffer, frame_size);
				ready = 0;
				bufptr = buffer;
			}
		}
	}
}

void Transmitter::sendPacket(Packet *p , int str_number)
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::sendPacket()"));
#endif
	int snt = 0;
	if (s != -1)
	{
		if(_enable_sctp){

			/*
			if( str_number == MAPLE_STM_AUDIO ){
				ttl = 100;
			}*/

//			debug("hang here? sctp_sendmsg");
			snt = ::sctp_sendmsg(s, p->getPacket(), p->getSize()
					, NULL , 0 , 0 , 0 , str_number , ttl , 0);
//			debug("no not here.");

			if( snt == EWOULDBLOCK ){
				debug(" sctp socket would block !!!!");
			}

		}else{
			snt = ::send(s, p->getPacket(), p->getSize(), MSG_NOSIGNAL);
		}

		if (snt <= 0)
		{
			emit finish();
		}
		else
		{
			bytes += snt;
			total += snt;
			ledOn(true);
		}
	}
	if (outFile) {
		fwrite(p->getPacket(), 1, p->getSize(), outFile);
		fflush(outFile);
	}
}

void Transmitter::send(float *sample, int samples)
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::send()"));
#endif
	if (isSpeaking(sample, samples)) 
	{
		if (status == TRANSMITTER_STATUS_WAITING)
			status = TRANSMITTER_STATUS_START;
		speak = stoptx; // To guarantee stream even for small speech pauses
	}
	switch (status)
	{
		case TRANSMITTER_STATUS_START:
			sendAudioPacket(prebuffer, frame_size);
			status = TRANSMITTER_STATUS_NORMAL;
		case TRANSMITTER_STATUS_NORMAL:
			if (speak > 0) {
				sendAudioPacket(sample, samples);
				speak--;
				break;
			}
			else 
			{
				status = TRANSMITTER_STATUS_WAITING;
			}
		case TRANSMITTER_STATUS_WAITING:  // It keeps the previous buffer to anticipate a little the speech
			memcpy(prebuffer, sample, samples*sizeof(float));
		case TRANSMITTER_STATUS_MUTE:
			ledOn(false);
			break;
	}
}

void Transmitter::ring(bool on)
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::ring(%1)").arg(on));
#endif
	if (on)
	{
		transmitting = false;
		ringing = true;
		sendRing();
	}
	else
	{
		ringing = false;
	}
}

void Transmitter::sendRing()
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::sendRing()"));
#endif
	if (ringing)
	{
		emit ringMessage();
		timer->start(RINGTIME, true);
		sendRingPacket();
	}
	else
	{
		try
		{
			prepare();
		}
		catch (Error e)
		{
			emitError(e.getText());
		}
	}
}

void Transmitter::sendAudioPacket(float *fsamples, int samples)
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::sendAudioPacket()"));
#endif
	int tot = 0;
	speex_bits_reset(&bits);
	if (state)
	{
		speex_encode(state, fsamples, &bits);
		tot = speex_bits_write(&bits, out, MAXBUFSIZE);
	}
	if (tot>0) {
		int size;
		char type = DRTA_INFO_AUDIO;
		try
		{
			if (blowfish)
			{
				size = PacketHandler::calculateCryptedSize(tot);
				type = DRTA_INFO_CRYPTED_AUDIO;
			}
			else
				size = PacketHandler::calculateSize(tot);
			Packet *p = new Packet (size);
			PacketHandler::buildModePacket(p, out, tot, type, speexmode);
			if (blowfish)
				p->crypt(blowfish);
				
			if( _enable_sctp) 
				sendPacket(p , MAPLE_STM_AUDIO);
			else
				sendPacket(p);
			delete p;
		}
		catch (Error e)
		{
			emitError(e.getText());
		}
	}
}

void Transmitter::emitError(QString text)
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::emitError(%1)").arg(text));
#endif
	emit error(text);
}

void Transmitter::sendSpecialPacket(char *data, int len, char type)
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::sendSpecialPacket()"));
#endif
	try
	{
		int size = PacketHandler::calculateSize(len);
		Packet *p = new Packet (size);
		PacketHandler::buildPacket(p, data, len, type);
		if( _enable_sctp) 
			sendPacket(p , MAPLE_STM_SPECIAL );
		else
			sendPacket(p);
		delete p;
	}
	catch (Error e)
	{
		emitError(e.getText());
	}
}

void Transmitter::sendNewPacket(char *data, int len, char type)
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::sendNewPacket()"));
#endif
	try
	{
		int size = PacketHandler::calculateSize(len);
		Packet *p = new Packet (size);
		PacketHandler::buildModePacket(p, data, len, speexmode, type);
		if( _enable_sctp) 
			sendPacket(p , MAPLE_STM_NEW);
		else
			sendPacket(p);
		delete p;
	}
	catch (Error e)
	{
		emitError(e.getText());
	}
}

void Transmitter::sendNamePacket(bool special, char type)
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::sendNamePacket()"));
#endif
	int dataLen = 0;
	char *dataPtr = NULL;
	if (!myName.isEmpty())
	{
		dataLen = myName.length()+1;
		dataPtr = (char *) myName.ascii();
	}
	if (special)
		sendSpecialPacket(dataPtr, dataLen, type);
	else
		sendNewPacket(dataPtr, dataLen, type);
}

void Transmitter::sendMsgPacket(QString msg)
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::sendMsgPacket(%1)").arg(msg));
#endif
	int dataLen = 0;
	char *dataPtr = NULL;

	QString buf = msg;
	if (!myName.isEmpty())
	{
		dataLen = buf .length()+1;
		dataPtr = (char *) buf .ascii();
	}
	sendNewPacket(dataPtr, dataLen, DRTA_INFO_MSG);

}


void Transmitter::answer()
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::answer()"));
#endif
	prepare();
	sendAnswerPacket();
}

void Transmitter::sendAnswerPacket()
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::sendAnswerPacket()"));
#endif
	sendNamePacket(false, DRTA_INFO_ANSWER);
}

void Transmitter::sendRingPacket()
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::sendRingPacket()"));
#endif
	sendNamePacket(false, DRTA_INFO_RING);
}

void Transmitter::sendRingReplyPacket()
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::sendRingReplyPacket()"));
#endif
	sendNamePacket(true, DRTA_INFO_RING_REPLY);
}

void Transmitter::sendRefusePacket()
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::sendRefusePacket()"));
#endif
	sendSpecialPacket(NULL, 0, DRTA_INFO_REFUSE);
}

void Transmitter::sendClosePacket()
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::sendClosePacket()"));
#endif
	sendSpecialPacket(NULL, 0, DRTA_INFO_CLOSE);
}

void Transmitter::sendResetPacket()
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::sendResetPacket()"));
#endif
	sendSpecialPacket(NULL, 0, DRTA_INFO_RESET);
}

void Transmitter::sendInitPacket()
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::sendInitPacket()"));
#endif
	sendNewPacket(NULL, 0, DRTA_INFO_INIT);
}

void Transmitter::sendKeyPacket()
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::sendKeyPacket()"));
#endif
	char *out;
	int keylen = rsa->getPeerPublicKeyLen();
	try
	{
		if (keylen > 0)
		{
			if (blowfish)
			{
				int len = rsa->encrypt(blowfish->getPass(), blowfish->getPassLen(), &out);
				sendSpecialPacket(out, len, DRTA_INFO_NEW_KEY);
				free(out);
			}
		}
		else
		{
			sendResetPacket();
		}
	}
	catch (Error e)
	{
		emitError(e.getText());
	}
}

void Transmitter::sendKeyRequestPacket()
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::sendKeyRequestPacket()"));
#endif
	int keylen = rsa->getMyPublicKeyLen();
	char *public_key = rsa->getMyPublicKey();
	sendSpecialPacket(public_key, keylen, DRTA_INFO_KEY_REQUEST);
}

void Transmitter::setThreshold(int th)
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::setThreshold(%1)").arg(th));
#endif
	threshold = (float) (th * th * 3.2);
}

long Transmitter::getBytes()
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::getBytes()"));
#endif
	long temp = bytes;
	bytes = 0;
	ledOn(false);
	return temp;
}

long Transmitter::getTotal()
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::getTotal()"));
#endif
	return total;
}

QString Transmitter::getIp()
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::getIp()"));
#endif
	return QString(inet_ntoa(sa.sin_addr));
}

void Transmitter::ledOn(bool on)
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::ledOn(%1)").arg(on));
#endif
	if (working)
		emit ledEnable(on);
}

bool Transmitter::isWorking()
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::isWorking()"));
#endif
	return working;
}

int Transmitter::Callx(QStrList host_list, int port, int prot)
{
#ifdef REN_DEBUG
	qWarning(QString("Transmitter::Callx()"));
#endif
	// Transmitter's initial
	init(prot);


	int sd = -1; // return value

	struct hostent *hostInfo = NULL;
	int adr_size = host_list . count();
	struct sockaddr_in *ip_list= new sockaddr_in[adr_size];
	

	for(int i = 0 ; i < adr_size ; i++){
		debug("Transmitter callx -  ip %d - %s",i , host_list.at(i));

		hostInfo = ::gethostbyname( host_list.at(i) );
		if(hostInfo == NULL)
			throw Error(tr("can't resolve ") + host_list.at(i));

		ip_list[i] . sin_family = hostInfo->h_addrtype;
		ip_list[i] . sin_port = htons(port);
		memcpy((char *) &ip_list[i] . sin_addr.s_addr,
				hostInfo->h_addr_list[0], hostInfo->h_length); 
	}

	int type = 0;
	
	switch(protocol)
	{
		case DRTA_UDP:
		case DRTA_TCP:
			throw Error("should not use callx");
			break;
		case DRTA_SCTP:
			type = SOCK_STREAM;
			break;
		case DRTA_SCTP_UDP:
			type = SOCK_SEQPACKET;
			break;
		default:
			throw Error("unknown protocol");
	}
	
	if ((sd = socket(AF_INET, type, IPPROTO_SCTP)) == -1)
		throw Error(tr("can't initalize socket (") + strerror(errno)+ tr(")"));

	SctpSocketHandler::SctpEnable(sd);
	SctpSocketHandler::SctpSetMaxStream(sd,5);
	SctpSocketHandler::SctpSetNoDelay(sd);

	SctpSocketHandler::SctpSetRtoMax(sd , 10000);
	SctpSocketHandler::SctpSetRtoMin(sd ,  1000);
	SctpSocketHandler::SctpTurnOnAllEvent(sd );

	if( protocol == DRTA_SCTP_UDP );
		SctpSocketHandler::SctpSetAutoClose(sd ,  true);
	
	if( ::sctp_connectx(sd , (struct sockaddr*) ip_list , adr_size ) < 0){
		debug("callx :: connnection refuse?"); 
		throw Error(strerror(errno));
	}
	
	//TODO : free IP_LIST
	delete ip_list;
	start(sd);
	
	initRecorder();
	
	return sd;
}
