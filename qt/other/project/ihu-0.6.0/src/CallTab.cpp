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

#include "CallTab.hpp"
#include "Config.h"

#include <qvariant.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qstring.h>
#include <qtooltip.h>
#include <qlistbox.h>
#include <qlineedit.h>
#include <qstatusbar.h>
#include <qstring.h>

#define STAT_TIME 1000

CallTab::CallTab( int id, QString hosts[], int maxhost, QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
	setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, sizePolicy().hasHeightForWidth() ) );

	callName = QString(name);

	QBoxLayout *mainLayout = new QVBoxLayout( this, 0, 10);

	mainFrame = new QFrame( this, "ledFrame" );
	mainLayout->addWidget(mainFrame);
	
	QBoxLayout *mLayout = new QVBoxLayout( mainFrame, 0, 10);

	mLayout->addSpacing(10);

	QBoxLayout *mhLayout = new QHBoxLayout(mLayout);
	
	hostname = new QLabel( mainFrame, "hostname" );
	QFont hostname_font(  hostname->font() );
	hostname_font.setPointSize( 12 );
	hostname_font.setBold( TRUE );
	hostname->setFont( hostname_font );
	
	mhLayout->addStretch();
	mhLayout->addWidget(hostname);
	mhLayout->addStretch();
	
	mLayout->addStretch();
	
	QBoxLayout *mhLayout0 = new QHBoxLayout(mLayout);

	hostEdit = new QComboBox( TRUE, mainFrame, "hostEdit" );
	hostEdit->setMinimumSize( QSize(100, 30) );
	hostEdit->setAutoCompletion(TRUE);
	hostEdit->setDuplicatesEnabled(FALSE);
	hostEdit->setInsertionPolicy(QComboBox::NoInsertion);
	hostEdit->clear();

	mhLayout0->addSpacing(5);
	mhLayout0->addWidget(hostEdit);
	mhLayout0->addSpacing(5);
	
	mLayout->addStretch();
	
	QBoxLayout *mhLayout1 = new QHBoxLayout(mLayout, 25);
	
	mhLayout1->addSpacing(5);

	callButton = new QPushButton( mainFrame, "callButton" );
	callButton->setMinimumSize( QSize(130, 35) );
	callButton->setIconSet( QIconSet( QPixmap::fromMimeSource( "call.png" ) ) );
	mhLayout1->addWidget(callButton, 2);
	
	ringButton = new QToolButton( mainFrame, "ringButton" );
	ringButton->setFixedSize( QSize(40, 35) );
	ringButton->setToggleButton(TRUE);
	ringButton->setIconSet( QIconSet( QPixmap::fromMimeSource( "bell.png" ) ) );
	ringButton->setEnabled(FALSE);
	mhLayout1->addWidget(ringButton, 1);
	
	stopButton = new QPushButton( mainFrame, "stopButton" );
	stopButton->setMinimumSize( QSize(130, 35) );
	stopButton->setEnabled( FALSE );
	stopButton->setIconSet( QIconSet( QPixmap::fromMimeSource( "hangup.png" ) ) );
	mhLayout1->addWidget(stopButton, 2);
	
	mhLayout1->addSpacing(5);

	mLayout->addStretch();
	
	QBoxLayout *mhLayout2 = new QHBoxLayout(mLayout);
	
	mhLayout2->addSpacing(30);
	
	muteMicButton = new QToolButton( mainFrame, "muteMicButton" );
	muteMicButton->setMinimumSize( QSize(30, 30) );
	muteMicButton->setToggleButton(TRUE);
	muteMicButton->setIconSet( QIconSet( QPixmap::fromMimeSource( "mic.png" ) ) );
	mhLayout2->addWidget(muteMicButton);

	mhLayout2->addStretch();

	ledFrame = new QFrame( mainFrame, "ledFrame" );
	ledFrame->setFrameStyle( QFrame::Panel | QFrame::Sunken );

	QBoxLayout *ledLayout = new QHBoxLayout(ledFrame, 3, 10);

	ledLayout->addSpacing(10);

	tx = new QLabel( ledFrame, "tx" );
	tx->setEnabled(FALSE);
	ledLayout->addWidget(tx);
	
	txPixmap = new QLabel( ledFrame, "txPixmap" );
	txPixmap->setPixmap( QPixmap::fromMimeSource( "red.png" ) );
	txPixmap->setEnabled(FALSE);
	ledLayout->addWidget(txPixmap);
	
	ledLayout->addSpacing(5);
	ledLayout->addStretch();

	lockPixmap = new QLabel( ledFrame, "lockPixmap" );
	lockPixmap->setPixmap( QPixmap::fromMimeSource( "lock.png" ) );
	lockPixmap->setEnabled( FALSE );
	ledLayout->addWidget(lockPixmap);

	ledLayout->addStretch();
	ledLayout->addSpacing(5);

	rxPixmap = new QLabel( ledFrame, "rxPixmap" );
	rxPixmap->setPixmap( QPixmap::fromMimeSource( "green.png" ) );
	rxPixmap->setEnabled(FALSE);
	ledLayout->addWidget(rxPixmap);

	rx = new QLabel( ledFrame, "rx" );
	rx->setEnabled(FALSE);
	ledLayout->addWidget(rx);
	
	ledLayout->addSpacing(10);

	mhLayout2->addWidget(ledFrame);

	mhLayout2->addStretch();

	muteSpkButton = new QToolButton( mainFrame, "muteSpkButton" );
	muteSpkButton->setMinimumSize( QSize(30, 30) );
	muteSpkButton->setToggleButton(TRUE);
	muteSpkButton->setIconSet( QIconSet( QPixmap::fromMimeSource( "speaker.png" ) ) );
	mhLayout2->addWidget(muteSpkButton);

	mhLayout2->addSpacing(30);

	statusbar = new QStatusBar(this, "statusbar");
	statusbar->setSizeGripEnabled(FALSE);

	QWidget *trafficWidget = new QWidget( this, "trafficWidget");
	QBoxLayout *tLayout = new QHBoxLayout(trafficWidget);
	trafficLabel = new QLabel( trafficWidget, "trafficLabel" );
	trafficLabel->setEnabled(FALSE);
	tLayout->addWidget(trafficLabel);
	statusbar->addWidget( trafficWidget, 0, TRUE);
	
	mainLayout->addWidget(statusbar);

	connect( ringButton, SIGNAL( clicked() ), this, SLOT( ringButtonClicked() ) );
	connect( callButton, SIGNAL( clicked() ), this, SLOT( callButtonClicked() ) );
	connect( stopButton, SIGNAL( clicked() ), this, SLOT( stopButtonClicked() ) );
	connect( hostEdit->lineEdit(), SIGNAL( returnPressed() ), this, SLOT( callButtonClicked() ) );
	connect( muteMicButton, SIGNAL( clicked() ), this, SLOT( muteMicButtonClicked() ) );
	connect( muteSpkButton, SIGNAL( clicked() ), this, SLOT( muteSpkButtonClicked() ) );

	languageChange();

	callId = id;
	received = false;
	talking = false;
	seconds = 0;
	skipStat = 0;

	for (int i=0; i<maxhost; i++)
	{
		if (!hosts[i].isEmpty())
			hostEdit->insertItem(hosts[i]);
	}
}

/*
 *  Destroys the object and frees any allocated resources
 */
CallTab::~CallTab()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void CallTab::languageChange()
{
	rx->setText( tr( "RX" ) );
	tx->setText( tr( "TX" ) );
	hostname->setText( tr( "Receiver address" ) );
	callButton->setText( tr( "&Call" ) );
	stopButton->setText( tr( "&Hang up" ) );
	ringButton->setText( QString::null );
	callButton->setAccel( QKeySequence( tr( "Alt+C" ) ) );
	ringButton->setAccel( QKeySequence( tr( "Alt+B" ) ) );
	stopButton->setAccel( QKeySequence( tr( "Alt+H" ) ) );
	trafficLabel->setText( tr( "0.0 KB/s" ) );

	QToolTip::add( hostEdit, tr( "Write the address (IP or DNS hostname) of the computer that you want to call.\nThen press Enter or click the Call button to start the communication." ) );
	QToolTip::add( txPixmap, tr( "This led is on when transmitting data" ) );
	QToolTip::add( rxPixmap, tr( "This led is on when receiving data" ) );
	QToolTip::add( callButton, tr( "Start the communication" ) );
	QToolTip::add( stopButton, tr( "Hang up" ) );
	QToolTip::add( ringButton, tr( "Ring!" ) );
	QToolTip::add( trafficLabel, tr( "Traffic" ) );
	QToolTip::add( lockPixmap, tr( "Encryption status" ) );
	QToolTip::add( muteMicButton, tr( "Disable audio input for this call" ) );
	QToolTip::add( muteSpkButton, tr( "Disable audio output for this call" ) );
}

void CallTab::answer()
{
	emit answerSignal(callId);
}

void CallTab::callButtonClicked()
{
	if (received)
		answer();
	else
		call(hostEdit->currentText());
}

void CallTab::call(QString host)
{
	if (hostEdit->listBox()->findItem(host) == 0)
		hostEdit->insertItem(host, 0);
	emit callSignal(callId, host);
}

void CallTab::startCall()
{
	callButton->setEnabled(FALSE);
	stopButton->setEnabled(TRUE);
	stopButton->setText("&Hang up");
	hostEdit->setEnabled(FALSE);
	hostname->setEnabled(FALSE);
	ringButton->setEnabled(TRUE);
	tx->setEnabled(TRUE);
	rx->setEnabled(TRUE);
	trafficLabel->setEnabled(TRUE);
	seconds = 0;
}

void CallTab::stopButtonClicked()
{
	callButton->setEnabled(FALSE);
	stopButton->setEnabled(FALSE);
	ringButton->setEnabled(FALSE);
	emit stopSignal(callId);
}

void CallTab::ringButtonClicked()
{
	bool on = false;
	if (ringButton->isOn())
		on = true;
	emit ringSignal(callId, on);
}

void CallTab::receivedCall(QString ip)
{
	hostEdit->setCurrentText(ip);
	hostEdit->setEnabled(FALSE);
	callButton->setText( tr("&Answer") );
	callButton->setAccel( QKeySequence( tr( "Alt+A" ) ) );
	callButton->setEnabled(TRUE);
	stopButton->setText( tr("&Refuse") );
	stopButton->setEnabled(TRUE);
	tx->setEnabled(TRUE);
	rx->setEnabled(TRUE);
	ledEnable(TRUE, TRUE);
	received = true;
}

void CallTab::connectedCall()
{
	if (ringButton->isOn())
		ringButton->setOn(FALSE);
	talking = true;
}

void CallTab::stopCall()
{
	callButton->setEnabled(TRUE);
	callButton->setText("&Call");
	callButton->setAccel( QKeySequence( tr( "Alt+C" ) ) );
	stopButton->setEnabled(FALSE);
	stopButton->setText("&Hang up");
	hostEdit->setEnabled(TRUE);
	hostname->setEnabled(TRUE);
	ringButton->setEnabled(FALSE);
	tx->setEnabled(FALSE);
	rx->setEnabled(FALSE);
	QToolTip::add( txPixmap, QString::null);
	QToolTip::add( rxPixmap, QString::null);
	if (ringButton->isOn())
		ringButton->setOn(FALSE);
	ledEnable(FALSE, FALSE);
	trafficLabel->setEnabled(FALSE);
	received = false;
	talking = false;
}

void CallTab::abortCall()
{
	stopCall();
}

void CallTab::message(QString text)
{
	skipStat = 3;
	statusbar->message(text, STAT_TIME*3);
}

void CallTab::crypt(bool on)
{
	lockPixmap->setEnabled(on);
}

void CallTab::clearHosts()
{
	QString tempText = hostEdit->currentText();
	hostEdit->clear();
	hostEdit->setEditText(tempText);
}

bool CallTab::isCrypted()
{
	return lockPixmap->isEnabled();
}

void CallTab::muteMicButtonClicked()
{
	bool on = muteMicButton->isOn();
	if (on)
	{
		message("Audio input disabled.");
		muteMicButton->setIconSet( QIconSet( QPixmap::fromMimeSource( "mic_mute.png" ) ) );
	}
	else 
	{
		message("Audio input enabled.");
		muteMicButton->setIconSet( QIconSet( QPixmap::fromMimeSource( "mic.png" ) ) );
	}
	emit muteMicSignal(callId, on);
}

void CallTab::muteSpkButtonClicked()
{
	bool on = muteSpkButton->isOn();
	if (on)
	{
		message("Audio output disabled.");
		muteSpkButton->setIconSet( QIconSet( QPixmap::fromMimeSource( "speaker_mute.png" ) ) );
	}
	else 
	{
		message("Audio output enabled.");
		muteSpkButton->setIconSet( QIconSet( QPixmap::fromMimeSource( "speaker.png" ) ) );
	}
	emit muteSpkSignal(callId, on);
}

void CallTab::addHost(QString host_name)
{
	if (!host_name.isEmpty())
		hostEdit->insertItem(host_name);
}

void CallTab::setCallId(int id)
{
	callId = id;
}

int CallTab::getCallId()
{
	return callId;
}

void CallTab::ledEnable(bool txOn, bool rxOn)
{
	txPixmap->setEnabled(txOn);
	rxPixmap->setEnabled(rxOn);
}

void CallTab::statistics(float traffic, QString statName)
{
	QString tempMsg;
	seconds++;
	trafficLabel->setText( QString("%1 KB/s").arg(traffic, 2, 'f', 1 ) );
	if (talking)
	{
		QString statTime;
		statTime.sprintf("%02d:%02d", (int)(seconds/60), (int)(seconds%60));
		tempMsg = QString("%1 - %2").arg(statName).arg(statTime);
	}

	if (skipStat > 0)
		skipStat--;
	else
		statusbar->message(tempMsg);
}

QString CallTab::getCallName()
{
	return callName;
}

QString CallTab::getCallButtonText()
{
	return callButton->text();
}

bool CallTab::isCallButtonEnabled()
{
	return callButton->isEnabled();
}

QString CallTab::getStopButtonText()
{
	return stopButton->text();
}

bool CallTab::isStopButtonEnabled()
{
	return stopButton->isEnabled();
}

void CallTab::setRingButton(bool on)
{
	ringButton->setOn(on);
}
