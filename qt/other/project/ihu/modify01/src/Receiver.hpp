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

#ifndef RECEIVER_HPP
#define RECEIVER_HPP

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <qobject.h>
#include <qstring.h>
#include <qtimer.h>
#include <qstringlist.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <qsocketnotifier.h>

#include "Config.h"
#include "Packet.h"
#include "Rsa.h"
#include "Blowfish.h"

class Receiver: public QObject {
	Q_OBJECT
public:

	enum stream_status {
		STREAM_OK = 0,
		STREAM_OUT_OF_SYNC,
		STREAM_MISSING_DATA,
		// renyang - stream要等著接收資料
		STREAM_READ_DATA,
	};
	
	enum signal_type {
		SIGNAL_INIT = 0,
		SIGNAL_RING,
		SIGNAL_RINGREPLY,
		SIGNAL_SENDNEWKEY,
		SIGNAL_KEYREQUEST,
		SIGNAL_NEWKEY,
		// renyang - 資料接收完成
		SIGNAL_FINISH
	};
	
	Receiver(Rsa *);
	~Receiver(void);
	void reset();
	void resetStream();
	void dump(QString);
	void listen(int, bool, bool);
	void putData(char *, int);
	void processData();
	bool processPacket(Packet *);
	long getBytes();
	long getTotal();
	void enableDecrypt(char *, int);
	void disableDecrypt();
	void close();
	void stop();
	void go();
	void noDecrypt();
	QString getIp();
	QString getCallerName();
	bool replied();
	bool refused();
	bool aborted();
	void emitError(QString);
	void flush();
	bool isDumping();
	void setConnected(bool);
	bool isConnected();
	void setReceived(bool);
	bool isReceived();
	bool isActive();
	// renyang-modify - 可以取得peer端的所有ip
	void getIps();
	// renyang-modify

private:
	// renyang - client socket file descriptor
	int s;
	stream_status sync;
	// renyang - client端的address
	struct sockaddr_in ca;
	// renyang - client端sockaddr_in的長度
	socklen_t calen;
	int port;
	QSocketNotifier* notifier;
	FILE *outFile;
	char *inputBuffer;
	char *streamBuffer;
	char *streamPtr;
	long bytes;
	long total;
	int streamLen;
	Blowfish *blowfish;
	Rsa *rsa;
	// renyang - 開始接收對方的資料, 包含對方要求通話的部分, 或是語音
	bool working;
	bool nodecrypt;
	bool ihu_refuse;
	bool ihu_reply;
	bool ihu_abort;
	// renyang - 表示是否接受對方的來電
	bool connected;
	// renyang - 表示是打電話過去的那一端還是等待別人打電話過來的那一端
	// renyang - 若是waiting這一端則received=true
	bool received;
	bool flushing;
	bool active;
	QTimer *checkTimer;
	QString callerName;
	int protocol;
	void emitSignal(signal_type);
	// renyang-modify - 用來記錄old primaddr
	QString primaddr;

public slots:
	void receive(void);
	void end();
	void start(int, int);
	void checkConnection();

signals:
	void finishSignal();
	void initSignal();
	void ringSignal();
	void error(QString);
	void keyRequest();
	void sendNewKey();
	void newKey(QString);
	void ringReplySignal();
	void message(QString);
	void warning(QString);
	void newSocket(int,int,struct sockaddr_in);
	void connectedSignal();
	void newAudioData(char*, int);
	// renyang-modify - 把對方的ips傳送給call
	void SignalgetIps(QStringList);
	// renyang-modify - 設定要送到對方的primary address
	void setPrimaddrSignal(QString);
	// renyang-modify - end
};

#endif
