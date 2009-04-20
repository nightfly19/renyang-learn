

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

#include "LogViewer.h"

#include <qvariant.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qtextedit.h>

LogViewer::LogViewer( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
	setName( "LogViewer" );
	setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, sizePolicy().hasHeightForWidth() ) );
	
 	QBoxLayout *logLayout = new QVBoxLayout( this, 5, 5); 
	
	textLog = new QTextEdit(this);
	textLog->setMinimumSize( QSize( 200, 80) );
	textLog->setTextFormat(Qt::LogText);
	QFont text_font(  textLog->font() );
	text_font.setFamily( "Courier" );
	text_font.setPointSize( 12 );
	textLog->setFont( text_font ); 
	
	clearButton = new QPushButton( this, "clearButton" );
	
	logLayout->addWidget( textLog, 2 );
	
	QBoxLayout *hLayout = new QHBoxLayout( logLayout ); 
	hLayout->addWidget(clearButton);
	
	languageChange();
	resize( QSize(700, 300).expandedTo(minimumSizeHint()) );
	clearWState( WState_Polished );
	
	// signals and slots connections
	connect( clearButton, SIGNAL( clicked() ), this, SLOT( clearLog() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
LogViewer::~LogViewer()
{
}


void LogViewer::clearLog()
{
	textLog->clear();
}

void LogViewer::addLog(QString text)
{
	textLog->append(text);
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void LogViewer::languageChange()
{
	setCaption( tr( "Drta Log Viewer" ) );
	clearButton->setText( tr( "Clear" ) );
}
