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

#include "SctpSocketHandler.h"

class Receiver: public QObject {
	Q_OBJECT
public:

	enum stream_status {
		// renyang - 應該表示至少有一個封包是可以讀取的
		STREAM_OK = 0,
		// renyang - 表示不是一個完整的Packet資料, Header本身就有問題啦
		STREAM_OUT_OF_SYNC,
		// renyang - 表示資料有遺失, 資料不完整
		STREAM_MISSING_DATA,
		// renyang - stream要等著接收資料(等著封包送進來)
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
	// renyang - streamPtr回到起始位址(streamBuffer)
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
	// renyang-modify - 設定預期是由哪一個ip送資料過來
	void setExpectAddress(QString);
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
	// renyang - streamBuffer與streamLen搭配起來, 是表示說目前把資料放到哪裡了
	char *streamBuffer;
	// renyang - 記錄讀取下一個Packet的起始位置
	char *streamPtr;
	// renyang - 用來記算接收的資料量(每次呼叫getBytes(), bytes會歸0)
	long bytes;
	// renyang - 用來記錄接收的總資料量, 呼叫完getTotal()不會歸0
	long total;
	// renyang - 表示目前收到但是, 還沒有處理的資料長度
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
	// renyang-modify - 用來處理sctp的事件
	void emitSctpEvent(void *);
	// renyang-modify - 每隔一段時間, 檢查是否有收到資料
	QTimer *checkReceiveTimer;

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
	// renyang-modify - 送出某一個peer ip的情況
	void SigAddressEvent(QString,QString);
	// renyang-modify - 對方要求影像
	void requestImage();
	// renyang-modify - 要求對方影像失敗
	void requestImageFail();
	// renyang-modify - 接收到片斷的Image
	void newVideoData(char *,int);
	// renyang-modify - 接收到對方要求要一個image的一部分
	void requestNextImage();
	// renyang-modify - 把接收到的整個資料放到qlabel_label中
	void completeImage();
	// renyang-modify - 當對方更改address ip時, 同時也會更改stream no
	void SigStreamNo(int);
	// renyang-modify - end
};

#endif
