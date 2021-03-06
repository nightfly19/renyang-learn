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

#ifndef ___IHUNOGUI_HPP___
#define ___IHUNOGUI_HPP___

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qobject.h>
#include <qstring.h>
#include <qtimer.h>

#include "Error.h"
#include "Config.h"
#include "FilePlayer.hpp"
#include "Phone.hpp"
#include "Logger.h"
#include "Rsa.h"

// renyang - 建立一個類別, 其類別是表示沒有GUI的IHU
class IhuNoGui : public QObject
{
	Q_OBJECT
public:
	IhuNoGui(Config&);
	~IhuNoGui();
	void initIhu();
	void startReceiver();
	void agcRefresh(bool);
	void adrRefresh(bool);
	// renyang - 打電話給某一個address
	void call(QString);
	void playFile(QString);
	void cryptOn();
	void cryptOn(int);
	void disableIn();
	void disableOut();
	void applySettings();
	void waitForCalls();
	void quit();

private:
	// renyang - 目的是建立一個與外部的configure檔是同一個的, 當修改其中一個, 另一個也是改變
	Config& ihuconfig;
	FilePlayer *fileplayer;
	Phone *phone;
	Logger *logger;
	QTimer *timer;
	bool autocrypt;
	bool showkey;

public slots:
	void keyRequest();
	void receivedCall(int id);
	void connectedCall(int id);
	void cancelCall(int id);
	void abortAll(QString);
	void message(QString);
	void message(int, QString);
	void abortCall(int, QString);
	void receivedNewKey(int, QString);
	void stopFile();
	void changeKey(int);
	void statistics();
};

#endif
