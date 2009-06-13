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

#ifndef TRANSMITTER_HPP
#define TRANSMITTER_HPP

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <qobject.h>
#include <qstring.h>
#include <qtimer.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "Packet.h"
#include "Rsa.h"
#include "Blowfish.h"

#include "SctpSocketHandler.h"

class Transmitter: public QObject {
	Q_OBJECT

public:

	enum signal_type {
		SIGNAL_FINISH = 0,
		SIGNAL_START,
		SIGNAL_RINGMESSAGE,
	};

	Transmitter(Rsa *);
	~Transmitter(void);
	void reset();
	void init(int);
	void setMyName(QString);
	void dump(QString);
	void start(int);
	int call(QString, int, int);
	void answer();
	void end();
	void go();
	void stop();
	void ring(bool);
	void setThreshold(int);
	long getBytes();
	long getTotal();
	void enableCrypt(char *, int);
	void disableCrypt();
	// renyang-modify - 預設是傳送特殊封包streamno為0, ttl為100
	void sendPacket(Packet *p,int streamno=0,int ttl=100);
	void sendNewPacket(char*, int, char);
	void sendSpecialPacket(char*, int, char,int,int ttl=100);
	void sendNamePacket(bool, char);
	void sendAudioPacket(char *, int);
	void sendInitPacket();
	void sendResetPacket();
	void sendAnswerPacket();
	void sendClosePacket();
	void sendRefusePacket();
	void sendRingPacket();
	void sendErrorPacket();
	void newConnection(int, struct sockaddr_in, int);
	QString getIp();
	void emitError(QString);
	bool isWorking();
	void prepare();
	bool isCrypted();
	bool isDumping();
	bool isActive();
	// renyang-modify - 送出一個特殊的封包, 表示目前的webcam有問題
	void sendVideoFailPacket();
	// renyang-modify - 表示image最後一個已經送完啦
	void sendVideoEndPacket();
	// renyang-modify - 送出含有image資料的封包
	void sendVideoPacket(char *,int);
	// renyang-modify - 要求傳送下一個部分的image
	void sendVideoNextPacket();
	// renyang-modify - 送出要求image的封包
	void sendVideoRequestPacket();
	// renyang-modify - 向指定的IP傳送ConfirmPacket
	void sendConfirmPacket();
	// renyang-modify - 傳送改變primary address的ip
	void sendPrimaryChangePacket();
	// renyang-modify - 更新到下一個stream number
	void updateStreamNo();

private:
	// renyang - 由client要傳送資料到server端要使用的socket
	int s;
	// renyang - server端的資訊
	struct sockaddr_in sa;
	socklen_t salen;
	int port;
	FILE *outFile;
	long bytes;
	long total;
	Blowfish *blowfish;
	Rsa *rsa;
	QString myName;
	QTimer *timer;
	int protocol;
	// renyang - 網路的部分client連線成功到server端
	bool working;
	// renayng - 是否要請peer端響鈴
	bool ringing;
	bool crypted;
	bool active;
	// renyang - 是否可以傳送資料
	bool tx;
	void emitSignal(signal_type);
	// renyang-modify - 設定一個base stream number值只有可能是0, 3, 6
	int base_stream_no;

public slots:
	void sendKeyRequestPacket();
	void sendKeyPacket();
	void sendRing();
	void sendRingReplyPacket();
signals:
	void finishSignal();
	void error(QString);
	void message(QString);
	void ringMessage();
	void startSignal();
};

#endif

