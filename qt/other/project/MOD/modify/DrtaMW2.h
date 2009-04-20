


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

#ifndef __DRTAMW2_H__
#define __DRTAMW2_H__

#ifdef HAVE_CONFIG_H
#include "config.h"	// renyang - 沒有config.h的這一個檔案,應該要讀取Config.h
#endif

#include <qvariant.h>
#include <qmainwindow.h>
#include <qtimer.h>
#include <qptrlist.h>

#include "Config.h"
#include "Logger.h"
#include "Receiver.h"
#include "Transmitter.h"
#include "Rsa.h"
#include "AudioPlayer.h"
#include "Recorder.h"

#include "DrtaMW.h"

class QFrame;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QAction;
class QActionGroup;
class QToolBar;
class QPopupMenu;
class QLabel;
class QComboBox;
class QPushButton;
class QSlider;
class QToolButton;

class LogViewer;

// renyang - 記錄對方的ip address
class PEER_ADDR{
	public:	
		PEER_ADDR(){	prim = false;	recv = false;	};
		PEER_ADDR(QString a , bool p){		addr = a;	prim = p;	recv = false;	};

		bool getRecv(){	return recv;	};
		void setRecv(bool r){	recv = r;	};

		bool getPrim(){	return prim;	};
		void setPrim(bool p){	prim= p;	};


		QString getAddr(){	return addr;	};
	private:
		QString addr;
		bool	prim;
		bool	recv;
};

//class DrtaMW2 : public QMainWindow
class DrtaMW2 : public DrtaMW 
{
	Q_OBJECT

public:
	enum Ui_State{
		UI_CALL,
		UI_WAITCALL_RECV,
		UI_WAITCALL_RECV_NO,
		UI_STOPALL,
		UI_PLAY_FILE,
		UI_ANSWER
	};

	void UiStateChange( Ui_State);

public:
	DrtaMW2 ( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
	~DrtaMW2();

	void languageChange();
	
	enum icon_type { DRTA_ICON_NORMAL = 0, DRTA_ICON_WAIT, DRTA_ICON_ALARM, DRTA_ICON_MISSED, DRTA_ICON_TALK };

	QStatusBar* statusbar;
	QWidget *lockWidget;
	AudioPlayer *player;
	Receiver *receiver;
	Recorder *recorder;
	Transmitter *transmitter;
	Rsa *rsa;
	Logger *logger;
	QTimer *timer;
	QTimer *restartTimer;
	LogViewer* logViewer;
public slots:
	virtual void initDrta(bool);
	virtual void closeEvent(QCloseEvent *);
	virtual void toggleVisibility();
	virtual void fileExit();
	virtual void helpContents();
	virtual void helpAbout();
	virtual void log();
	virtual void settings();
	virtual void waitCalls(bool);
	virtual void waitForCalls();
	virtual void answer();
	virtual void call(QString);
	virtual void Callx(); // sctp connectx
	virtual void startAll();
	virtual void stopAll();
	virtual void stop();
	virtual void stopSignal();
	virtual void adrOn(bool);
	virtual void agcOn(bool);
	virtual void adrRefresh(bool);
	virtual void agcRefresh(bool);
	virtual void cryptOn();
	virtual void crypt(bool);
	virtual bool changeKey();
	virtual void setDecryptionKey();
	virtual void agcSliderChanged(int);
	virtual void sliderChanged(int);
	virtual void play();
	virtual void playFile(QString);
	virtual void abortAll(QString);
	virtual void showMessageCritical(QString);
	virtual void showWarning(QString);
	virtual void message(QString);
	virtual void ringMessage();
	virtual void warning(QString);
	virtual void sendKey();
	virtual void sendKeyRequest();
	virtual void newConnection(int, int, struct sockaddr_in);
	virtual void transmitterStatus(bool);
	virtual void receiverStatus(bool);
	virtual void ringOn(bool);
	virtual void changeProgress(int);
	virtual void sliderPress();
	virtual void sliderRelease();
	virtual void txLedEnable(bool);
	virtual void rxLedEnable(bool);
	virtual void applySettings();
	virtual void disableIn();
	virtual void disableOut();
	virtual void listen();
	virtual void receivedNewKey(QString);

	virtual void slotIPAdd();
	virtual void slotIPDel();
	virtual void slotSendMsg();

	virtual void slotRecvMsg(QString);
	virtual void slotRecvFrom(QString);
	virtual void slotSetSctp();
	virtual void slotSctpEvent(QString);
	virtual void slotRemoteTerminate();

	virtual void SctpRefreshRemoteIP(int);
	virtual void slotSetPrim();
	virtual void slotSctpClear();
	virtual void slotPrSendFailed();
private:
	bool fromFile;
	bool sliderFree;
	bool listening;
	bool received;
	bool tocrypt;
	bool closing;
	int skipStat;
	unsigned long seconds;
	QString fileName;
	Config& config;	// renyang - 建立一個alias的變數

	QLabel *lockPixmap;
	QLabel* trafficLabel;
	QPtrList<PEER_ADDR> peer_addr_lst;	// renyang - 記錄所有PEER_ADDR的資料, 準備當線路斷掉時切換

	/* sctp */
	bool allow_auto_ho;
	int error_threshold;

	int hand_over_policy;
	int reliable_path;

protected slots:
	virtual void statistics();
};

#endif
