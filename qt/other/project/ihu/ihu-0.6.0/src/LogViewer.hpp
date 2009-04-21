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

#ifndef LOGVIEWER_H
#define LOGVIEWER_H

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
	QPushButton* saveButton;
	QPushButton* closeButton;
	QTextEdit* textLog;

public slots:
	virtual void addLog(QString);
	virtual void clearLog();
	virtual void saveLog();
	virtual void closeLog();

protected slots:
	virtual void languageChange();

private:

};

#endif
