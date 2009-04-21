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

#ifndef PLAYERDIALOG_H
#define PLAYERDIALOG_H

#include "FilePlayer.hpp"

#include <qvariant.h>
#include <qdialog.h>
#include <qstring.h>

class QFrame;
class QPushButton;
class QLabel;
class QSlider;
class QStatusBar;
class QProgressBar;
class QTimer;

class PlayerDialog : public QDialog
{
	Q_OBJECT

public:
	PlayerDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
	~PlayerDialog();
	QFrame *playerFrame;
	QPushButton* openButton;
	QPushButton* convertButton;
	QPushButton* pauseButton;
	QPushButton* stopButton;
	QSlider* slider;
	QStatusBar* statusbar;
	QTimer *statTimer;
	QProgressBar* progressBar;

	FilePlayer* fileplayer;

	bool sliderFree;
 	bool ihu2spx;
 	QString fileName;

	void setupPlayer(QString);

public slots:
	virtual void playFile();
	virtual void playFile(QString);
	virtual void convertFile();
	virtual void stopFile();
	virtual void pauseButtonToggled(bool);
	virtual void stopButtonClicked();
	virtual void warning(QString);
	virtual void abort(QString);
	virtual void sliderPress();
	virtual void sliderRelease();
	virtual void keyRequest();
	virtual void changeProgress(int);
	virtual void statistics();
	virtual void message(QString);

protected slots:
	virtual void languageChange();

private:

};

#endif
