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

#ifndef IHU_HPP
#define IHU_HPP

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qvariant.h>
#include <qmainwindow.h>
#include <qtimer.h>

#include "Config.h"
#include "PlayerDialog.hpp"
#include "Phone.hpp"
#include "Logger.h"
#include "CallTab.hpp"
#include "Rsa.h"

#include "trayicon.hpp"

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
class QTabWidget;
class QProgressBar;
class QStatusBar;

class LogViewer;

class Ihu : public QMainWindow
{
	Q_OBJECT

public:
	Ihu( Config&, QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
	~Ihu();
	
	enum icon_type { IHU_ICON_NORMAL = 0, IHU_ICON_WAIT, IHU_ICON_ALARM, IHU_ICON_TALK };

	QStatusBar* statusbar;
	QTabWidget *callWidget;
	QFrame *callFrame;
	QFrame *waitFrame;
	QFrame *otherFrame;
	QLabel* threshold;
	QLabel* trafficLabel;
	QLabel* soundLabel;
	QLabel* delayLabel;
	QPushButton* waitButton;
	QSlider* thSlider;
	QProgressBar* soundLevel;
	QToolButton* muteSpkButton;
	QToolButton* muteMicButton;
	QMenuBar *menubar;
	QPopupMenu *fileMenu;
	QPopupMenu *callMenu;
	QPopupMenu *optionsMenu;
	QPopupMenu *helpMenu;
	QAction* newCallAction;
	QAction* closeCallAction;
	QAction* filePlayFileAction;
	QAction* logAction;
	QAction* clearhAction;
	QAction* clearmAction;
	QAction* fileQuitAction;
	QAction* helpContentsAction;
	QAction* helpAboutAction;
	QAction* settingsAction;
	QAction* adrAction;
	QAction* agcAction;
	QAction* cryptAction;
	QAction* changeKeyAction;
	QAction* setDecKeyAction;
	QAction* dumpRXAction;
	QAction* dumpTXAction;
	PlayerDialog *playerDialog;
	Phone* phone;
	Logger *logger;
	QTimer *timer;
	LogViewer* logViewer;
	TrayIcon* trayIcon;

	int maxcalls;
	CallTab **callTab;

public slots:
	virtual void initIhu();
	virtual void closeEvent(QCloseEvent *);
	virtual void toggleVisibility();
	virtual void trayMenuRequested( const QPoint& );
	virtual void changeTrayIcon(icon_type);
	virtual void quit();
	virtual void helpContents();
	virtual void helpAbout();
	virtual void showLogViewer();
	virtual void clearHistory();
	virtual void clearMissed();
	virtual void settings();
	virtual void adrOn();
	virtual void agcOn();
	virtual void adrRefresh(bool);
	virtual void agcRefresh(bool);
	virtual void crypt();
	virtual void crypt(int, bool);
	virtual void changeKey();
	virtual bool changeKey(int);
	virtual void setDecryptionKey();
	virtual void setDecryptionKey(int);
	virtual void enableRecorder(bool);
	virtual void enablePlayer(bool);
	virtual void thSliderChanged(int);
	virtual void showPlayer();
	virtual void showMessageCritical(QString);
	virtual void showWarning(QString);
	virtual void message(QString);
	virtual void message(int, QString);
	virtual void warning(QString);
	virtual void warning(int, QString);
	virtual void recorderStatus(bool);
	virtual void playerStatus(bool);
	virtual void applySettings();
	virtual void playFile(QString);
	virtual void toCrypt(int);
	virtual void cryptOn();
	virtual void disableIn();
	virtual void disableOut();
	virtual void receivedNewKey(int, QString);
	virtual void receivedCall(int);
	virtual void connectedCall(int);
	virtual void menuUpdate();
	virtual void dumpRXEnable();
	virtual void dumpTXEnable();
	virtual QString getFileName();
	virtual void waitButtonClicked();
	virtual void waitForCalls();

	virtual void answerCall(int);
	virtual void call(QString);
	virtual void call(int, QString);
	virtual void stopCall(int);
	virtual void cancelCall(int);
	virtual void abortCall(int, QString);
	virtual void newCall();
	virtual void newCall(int);
	virtual void closeCall();
	virtual void ring(int, bool);
	virtual void muteMic(int, bool);
	virtual void muteSpk(int, bool);
	virtual void abortAll(QString);
	virtual void abortWait(QString);

private:
	bool closing;
	bool autoanswer;
	bool autocrypt;
	bool showkey;
	int skipStat;
	Config& ihuconfig;
	icon_type icon_status;

protected slots:
	virtual void languageChange();
	virtual void statistics();
};

#endif
