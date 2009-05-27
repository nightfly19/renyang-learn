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
	// renyang-modify - end

private:
	int callId;
	bool received;
	// renyang - 表示目前正在通話啦
	bool talking;
	unsigned long seconds;
	int skipStat;
	QString callName;

public slots:
	virtual void callButtonClicked();
	virtual void stopButtonClicked();
	virtual void ringButtonClicked();
	virtual void message(QString);
	virtual void muteMicButtonClicked();
	virtual void muteSpkButtonClicked();
	// renyang-modify - 處理按下primButton的事件
	void primButtonClicked();
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
};

#endif
