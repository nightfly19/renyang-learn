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

#ifndef CALL_HPP
#define CALL_HPP

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <qobject.h>
#include <qstring.h>
#include <qtimer.h>
#include <qimage.h>

#include <speex/speex.h>

#include "Rsa.h"
#include "Receiver.hpp"
#include "Transmitter.hpp"
// renyang-modify - 增加一個SctpIPHandler, 處理多個ip的類別
#include "SctpIPHandler.hpp"

#define MAXDATA 480*640*3

// renyang-modify - 用來記錄要要接收與傳送的影像的資料結構
struct image_matrix
{
	int width,height;
	char data[MAXDATA];
};

class Call: public QObject {
	Q_OBJECT
public:

	Call(int, QString);
	~Call(void);
	void start(int, int);
	int getId();
	QString getCallerName();
	QString getCallerIp();
	void enableRandomCrypt(int);
	void enableCrypt(char *, int);
	void enableDecrypt(char *, int);
	void disableCrypt();
	void disableDecrypt();
	void answer();
	void end();
	void stop();
	void error();
	void sendRing(bool);
	// renyang - 要撥打出去的電話(ip address, port, protype)
	void call(QString, int, int);
	long getTraffic();
	long getTotalTX();
	long getTotalRX();
	long getTotal();
	// renyang - 把資料由網路傳送出去
	void send(char*, int);
	void muteRecorder(bool);
	void mutePlayer(bool);
	void dumpRXStream(QString);
	void dumpTXStream(QString);
	bool isDumpingRX();
	bool isDumpingTX();
	bool isRXActive();
	bool isTXActive();
	bool isFree();
	void setMyName(QString);
	void updateFrames(int);
	void putData(float*, int);
	bool isRecording();
	// renyang-modify - 表示本機端的webcam有問題
	void sendVideoFail();
	// renyang-modify - 停止要求對方的video
	void requestPeerImageStop();

private:
	// renyang - 表示目前call id
	int id;
	// renyang - 表示peer端的socket
	int sd;

	Rsa *rsa;
	// renyang - 接收端
	Receiver *receiver;
	// renyang - 傳送端
	Transmitter *transmitter;

	// renyang-modify - 用來處理sctp的ip
	SctpIPHandler *sctpiphandler;

	// renyang - 存放SpeexBits
	SpeexBits bits;
	void *dec_state;
	// renyang - 由Packet解碼的音訊
	float *outBuffer;
	// renyang - 存放以經解完碼的音訊
	float *soundBuffer;
	// renyang - 記錄每一個frame至少要多少個bytes才能代表
	int frame_size;
	// renyang - 目前有多少個frame已經可以播放了
	int readyFrames;
	// renyang-modify - 記錄目前是否是在跟對方要求影像, 而還是等待剩下影像部分的情況
	bool requestingVideo;

	int inport;
	bool udp;
	bool tcp;
	// renyang - 顯示目前是否有由音效卡取得音效
	bool recording;
	// renyang - 表示目前這一個call是否正在撥打(進行中)
	bool active;
	bool muteRec;
	// renyang - 目前這一個Call是否為靜音設定
	bool mutePlay;
	// renyang - 表示目前這一個Call是否正在被使用(撥電話出去, 等待接電話, 通話中都算是false)
	bool callFree;
	// renyang - 目前這一個call出現錯誤
	bool aborted;
	QTimer *stopTimer;
	// renyang-modify - 用來接收的image的資料
	struct image_matrix RecvImage;
	// renyang-modify - 用來存放要送出去的image資料
	struct image_matrix SendImage;
	// renyang-modify - 用來記錄接收到哪一個部分啦(因為我們把一個封包分成許多個部分)
	int recvImage_index;
	// renyang-modify - 用來記錄傳送的封包傳送到哪一個部分啦
	int sendImage_index;
	// renyang-modify - 表示這一個frame剩下多少個資料還沒有送完
	int send_left;
	// renyang-modify - 存放要放上去的image
	QImage image;
	// renyang-modify - 每當要求一個frame時，在一定的時間內沒有回傳回來的話，則要求下一個frame
	QTimer *videoTimer;

public slots:
	void newConnection(int, int, struct sockaddr_in);
	void sendKeyRequest();
	void sendKey();
	void receivedNewKey(QString);
	void stopCall();
	void message(QString);
	void warning(QString);
	void abortCall(QString);
	void ringMessage();
	void startRecorder();
	// renyang - 連線成功
	void connected();
	void close();
	void decodeAudioData(char*, int);
	bool playData(float*, int);
	bool mixData(float*, int, float);
	void playRing();
	void playInit();
	// renyang-modify - 接收由call傳送上來的peer address
	void SlotgetIps(QStringList);
	void setPrimaddr(QString);
	// renyang-modify - 接收由Receiver傳送上來針對每一個ip的事件
	void SlotAddressEvent(QString,QString);
	// renyang-modify - 由webcam取得image，並放入SendImage中
	void SlotGetImage();
	// renyang-modify - 處理視訊資料
	void decodeVideoData(char *, int);
	// renyang-modify - 開始傳送影像資料啦
	void sendVideo();
	// renyang-modify - 接收到完整的image，準備把它放到video_label
	void processImage();
	// renyang-modify - 向對方要求影像失敗
	void SlotrequestImageFail();
	// renyang-modify - 像某一個ip傳送封包
	void SlotAddressConfirm(QString);
	// renyang-modify - 表示這一個ip宣告為無法連線
	void SlotAddressFail(QString);
	// renyang-modify - 設定某一個ip原本不能使用, 現在可以使用啦
	void SlotAddressAvailable(QString);
	// renyang-modify - 向peer端傳送要求video的封包
	void sendVideoRequest();
	// renyang-modify - end

signals:
	void connectedSignal(int);
	void receivedSignal(int);
	void cancelCallSignal(int);
	void abortSignal(int, QString);
	void newKeySignal(int, QString);
	void warningSignal(int, QString);
	void cryptedSignal(int);
	void ringSignal(int);
	void initSignal(int);
	// renyang-modify - 傳送由peer address到phone
	void SignalgetIps(int,QStringList);
	// renyang-modify - 送出某一個peer ip的情況
	void SigAddressEvent(int,QString,QString);
	void SigGetImage(char *,int);
	void SigputImage(QImage,int);
	// renyang-modify - 向對方要求影像失敗
	void SigrequestImageFail(int);
	// renyang-modify - end
};

#endif
