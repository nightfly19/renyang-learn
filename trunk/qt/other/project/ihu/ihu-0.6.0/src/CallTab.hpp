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

	void addHost(QString);
	void clearHosts();

	void setCallId(int);
	int getCallId();
	void ledEnable(bool, bool);
	void statistics(float, QString);
	void answer();
	void call();
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

private:
	int callId;
	bool received;
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

protected slots:
	virtual void languageChange();

signals:
	void callSignal(int, QString);
	void answerSignal(int);
	void stopSignal(int);
	void ringSignal(int, bool);
	void muteMicSignal(int, bool);
	void muteSpkSignal(int, bool);
};

#endif
