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

#ifndef PHONE_HPP
#define PHONE_HPP

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <qobject.h>
#include <qtimer.h>
#include <qstring.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <qsocketnotifier.h>

#include <speex/speex.h>

#include "Call.hpp"
#include "Player.hpp"
#include "Recorder.hpp"
#include "TcpServer.hpp"

#include "ring_sounds.h"

class Phone: public QObject {
	Q_OBJECT
public:

	enum recorder_status {
		RECORDER_STATUS_RUNNING = 0,
		RECORDER_STATUS_START,
		RECORDER_STATUS_STOP,
		RECORDER_STATUS_WAITING,
		RECORDER_STATUS_MUTE
	};

	enum player_status {
		PLAYER_STATUS_RUNNING = 0,
		PLAYER_STATUS_STOP,
		PLAYER_STATUS_MUTE
	};

	Phone(int);
	~Phone();

	void resize(int);
	// renyang - 等待別人連線
	void waitCalls(int, bool, bool);
	// renyang - 與server端建立連線
	void call(int, QString, int, int);
	void answerCall(int);
	void endAll();
	void endCall(int);
	void abortAll();
	void stopWaiting();
	int createCall();
	int newCall();
	void deleteCall(int);
	void ring(int, bool);
	void receivedCall(int);

	void send(float*, int);
	// renyang - 傳送音訊資料
	void sendAudioData(float *, int);

	void startRecorder();
	void stopRecorder();
	void startPlayer();
	void stopPlayer();

	// renyang - 設定語音的編碼設定
	void setup(int, int, int, int, float, int, int, int, int, int, int);
	void setupRecorder(int, QString);
	void setupPlayer(QString, int);
	void setupAdr(bool, float, float, float);
	void setupAgc(bool, bool, bool, float, float, const char*);
	void disableRecorder(bool);
	void disablePlayer(bool);
	void mutePlayer(int, bool);
	void muteRecorder(int, bool);

	void enableRandomCrypt(int, int);
	void enableDecrypt(int, char *, int);
	void enableCrypt(int, char *, int);
	void disableDecrypt(int);
	void disableCrypt(int);

	int getConnections();
	int getCalls();
	long getCallTraffic(int);
	long getCallTX(int);
	long getCallRX(int);
	long getTotal();
	QString getCallerName(int);
	QString getCallerIp(int);
	float getDelay();

	void dumpRXStream(int, QString);
	void dumpTXStream(int, QString);
	bool isDumpingRX(int);
	bool isDumpingTX(int);
	bool isRXActive(int);
	bool isTXActive(int);
	void setMyName(QString);
	void setThreshold(int);

	// renyang - 尋找出最大音量, 來確認是有在說話
	bool isSpeaking(float*, int);
	int findFreeId();
	int findFreeCall();
	void checkSound();
	bool isListening();
	void clearConnections();
	int getPeak();

private:

	// renyang - rec_status共有以下狀態
	// renyang - RECORDER_STATUS_WAITING, RECORDER_STATUS_START, RECORDER_STATUS_RUNNING
	// renyang - RECORDER_STATUS_MUTE, RECORDER_STATUS_STOP
	recorder_status rec_status;
	player_status play_status;

	Recorder *recorder;
	Player *player;

	QTimer *timer;

	// renyang - 記錄所有要撥打出去的電話
	Call **calls;
	// renyang - 最大可以有多少call同時連線(包括別人打過來，我們打過去，通話中與響鈴中)
	int maxcall;
	int call_number;
	// renyang - 目前由client端連線到server端的個數
	// renyang - 並不是表示server端接受了client端的電話邀請
	// renyang - 只是純粹網路連接起來了而以
	int connections;

	// renyang - 用來處理是否有新的tcp連線進來
	TcpServer *tcpserver;
	// renyang - 用來記錄udp的server端代表server的socket@@@
	struct sockaddr_in sa;
	// renyang - 當有資料進來時, 表示有新的連線, 目前應該是設定給udp使用的
	QSocketNotifier* notifier;
	// renyang - server端的udp socket
	int sd;
	// renyang - 對方要連進來要使用的port號
	int inport;

	// renyang - modify - start
	struct sockaddr_in sctp_sa;
	QSocketNotifier *sctp_notifier;
	int sctp_sd;
	// renyang - modify - end

	SpeexBits enc_bits;
	void *enc_state;

	float *recBuffer;
	// renyang - 還要讀多少資料才可以送出音訊封包
	int toRead;
	float *buffer, *bufptr;
	float *prebuffer;
	// renyang - 存放要撥放的音訊的buffer起始位置
	float *playBuffer;
	char *out;
	// renyang - 存在playBuffer的frame個數
	int frame_size;
	int ready;
	int stoptx;
	int speak;
	int rate;
	float threshold;
	float peak;
	int play_rate;
	int play_frame_size;
	float balance;

	bool recording;
	// renyang - 放出聲音啦(撥放器開始運作啦)
	bool playing;
	// renyang - 是否在server端建立了socket等待client的連線
	bool listening;

	QString myName;

	bool agc;
	bool agc_hw;
	bool agc_sw;
	float agc_level;
	float agc_step;
	float amp;
	float agc_err;

	float ringBuffer[SIZE_RING_32];
	// renyang - 是否有要響鈴
	bool ringing;
	int ringPtr;
	// renyang - 還要響鈴的次數
	int ringCount;

public slots:
	void warning(QString);
	void newUDPConnection(int);
	void newTCPConnection(int);
	void connectedCall(int);
	void cancelCall(int);
	void stopCall(int);
	void abortCall(int);
	void abortCall(int, QString);
	void callWarning(int, QString);
	void receivedNewKey(int, QString);
	void processAudioSamples(float*, int);
	void cryptCall(int);
	void playInit(int);
	void playRing(int);
	void playCallback();
	// renyang - modify - start
	void newSCTPConnection(int);
	void renotification_sctp();
	void re_notification_sctp();
	// renyang - modify - end

signals:
	void warningSignal(QString);
	void messageSignal(int, QString);
	void newKeySignal(int, QString);
	void abortSignal(QString);
	void abortCallSignal(int, QString);
	void receivedCallSignal(int);
	void connectedCallSignal(int);
	void cancelCallSignal(int);
	void cryptedSignal(int);
	void newCallSignal(int);
	void recorderSignal(bool);
	void playerSignal(bool);
};

#endif
