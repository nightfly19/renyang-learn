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

#ifndef CALLTAB_H
#define CALLTAB_H

#include <qobject.h>
#include <qvariant.h>
#include <qwidget.h>
#include <qimage.h>
#include <qcheckbox.h>
#include <qtimer.h>

class QFrame;
class QLabel;
class QComboBox;
class QPushButton;
class QToolButton;
class QStatusBar;
// renyang-modify
class QListBox;
// renyang-modify

class CallTab : public QWidget
{
	Q_OBJECT

public:
	CallTab( int, QString[], int, QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
	~CallTab();

	QStatusBar* statusbar;

	QFrame* mainFrame;
	QFrame* ledFrame;
	QLabel* hostname;
	QLabel* tx;
	QLabel* rx;
	QComboBox* hostEdit;
	QPushButton* callButton;
	QPushButton* stopButton;
	QToolButton* ringButton;
	QLabel *rxPixmap;
	QLabel *txPixmap;
	QLabel* trafficLabel;
	QLabel *lockPixmap;
	QToolButton* muteSpkButton;
	QToolButton* muteMicButton;
	// renyang-modify - 增加host list的指標
	QListBox *hostList;
	QPushButton *primButton;
	// renyang-modify - end

	void addHost(QString);
	void clearHosts();

	void setCallId(int);
	int getCallId();
	void ledEnable(bool, bool);
	void statistics(float, QString);
	void answer();
	void call();
	// renyang - 打電話給指定的host
	void call(QString);
	void receivedCall(QString);
	void connectedCall();
	void stopCall();
	void startCall();
	void abortCall();
	void crypt(bool);
	bool isCrypted();
	QString getCallName();

	QString getCallButtonText();
	QString getStopButtonText();
	bool isCallButtonEnabled();
	bool isStopButtonEnabled();
	void setRingButton(bool);

	// renyang-modify - 改變hostList的內容
	void change_hostList(QStringList addrs_list);
	// renyang-modify - 依某一個ip的情況來改變hostList的顯示
	void setAddressEvent(QString,QString);
	// renyang-modify - 用來處理當send fail的的作為
	void SendFailedHandler();
	// renyang-modify - 送出Image給video_label
	void setVideo(QImage);
	// renyang-modify - 向對方要求影像失敗
	void requestImageFail();
	// renyang-modify - 清除video_label的內容
	void clearVideoLabel();
	// renyang-modify - end

private:
	int callId;
	bool received;
	// renyang - 表示目前正在通話啦
	bool talking;
	unsigned long seconds;
	int skipStat;
	QString callName;
	// renyang-modify - 目前的send fail threshold
	int error_threshold;
	// renyang-modify - 記錄目前send fail 累積到幾次啦
	int error_handled;
	// renyang-modify - 顯示影像, 把影像放到video_label上
	QLabel *video_label;
	// renyang-modify - 決定是否要要求對方送影像過來
	QCheckBox *video_check;
	// renyang-modify - 當要向對方要求影像, 但是, 像還沒有整個上傳到CallTab時, waiting是true, 用來處理fps的
	bool waiting;
	// renyang-modify - 記錄對方的video是否可以被要求
	bool peervideofail;
	QTimer *video_timer;
	// renyang-modify - 記錄前一個的primary index
	int previous_primaddr_index;

public slots:
	virtual void callButtonClicked();
	virtual void stopButtonClicked();
	virtual void ringButtonClicked();
	virtual void message(QString);
	virtual void muteMicButtonClicked();
	virtual void muteSpkButtonClicked();
	// renyang-modify - 處理按下primButton的事件
	void primButtonClicked();
	// renyang-modify - 當CheckChanged有改變時, 所要做的事
	void videoCheckChanged();
	// renyang-modify - 當video_timer時間到時要執行的function
	void video_timeout();
	// renyang-modify - end

protected slots:
	virtual void languageChange();

signals:
	void callSignal(int, QString);
	void answerSignal(int);
	void stopSignal(int);
	void ringSignal(int, bool);
	void muteMicSignal(int, bool);
	void muteSpkSignal(int, bool);
	// renyang-modify - 設定primary address
	void setPrimaddrSignal(int,QString);
	// renyang-modify - 向對方要求一個影像
	void SigrequestPeerImage(int);
	// renyang-modify - 停止要求對方的影像
	void SigrequestPeerImageStop(int);
	// renyang-modify - 告知SctpIPHandler某一個ip相聯的情況
	void SigAddressInfo(int,QString,bool);
};

#endif
