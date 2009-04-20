

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

#ifndef __LOGVIEWER_H__
#define __LOGVIEWER_H__

#include <qvariant.h>
#include <qdialog.h>

class QPushButton;
class QLabel;
class QTextEdit;

class LogViewer : public QDialog
{
	Q_OBJECT

public:
	LogViewer( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
	~LogViewer();
	
	QPushButton* clearButton;
	QTextEdit* textLog;

public slots:
	virtual void clearLog();
	virtual void addLog(QString);

protected slots:
	virtual void languageChange();

private:

};

#endif
