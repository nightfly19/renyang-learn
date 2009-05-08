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

#include "LogViewer.hpp"
#include <errno.h>

#include <qvariant.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qtextedit.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qfiledialog.h>
#include <qmessagebox.h>

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
	saveButton = new QPushButton( this, "saveButton" );
	closeButton = new QPushButton( this, "closeButton" );
	
	logLayout->addWidget( textLog, 2 );
	
	QBoxLayout *hLayout = new QHBoxLayout( logLayout ); 
	hLayout->addWidget(clearButton);
	hLayout->addWidget(saveButton);
	hLayout->addWidget(closeButton);
	
	languageChange();
	resize( QSize(700, 300).expandedTo(minimumSizeHint()) );
	clearWState( WState_Polished );
	
	// signals and slots connections
	connect( clearButton, SIGNAL( clicked() ), this, SLOT( clearLog() ) );
	connect( saveButton, SIGNAL( clicked() ), this, SLOT( saveLog() ) );
	connect( closeButton, SIGNAL( clicked() ), this, SLOT( closeLog() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
LogViewer::~LogViewer()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void LogViewer::languageChange()
{
	setCaption( tr( "IHU Log Viewer" ) );
	clearButton->setText( tr( "Clear" ) );
	saveButton->setText( tr( "Save" ) );
	saveButton->setIconSet( QIconSet( QPixmap::fromMimeSource( "save.png" ) ) );
	closeButton->setText( tr( "Close" ) );
}

void LogViewer::clearLog()
{
	textLog->clear();
}

void LogViewer::addLog(QString text)
{
	textLog->append(text);
}

void LogViewer::saveLog()
{
	QString name = QFileDialog::getSaveFileName("", "*.*", this, 0, "Save to file...");
	if (!name.isEmpty())
	{
		FILE *logfile = fopen(name.ascii(), "a");
		if (logfile==NULL)
		{
			QMessageBox::critical(this, tr("Write failed"), tr("Could not open file for writing: %1").arg(strerror(errno)) );
		}
		else
		{
			QTextStream(logfile, IO_WriteOnly) << textLog->text();
			fclose(logfile);
		}
	}
}

void LogViewer::closeLog()
{
	hide();
}
