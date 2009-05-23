


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

#include <stdlib.h>
#include <time.h>

#include <qvariant.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qfiledialog.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qmessagebox.h>
#include <qstatusbar.h>
#include <qinputdialog.h>
#include <qcombobox.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qstring.h>

#include "DrtaMW2.h"
#include "Settings.h"
#include "LogViewer.h"
#include "Error.h"

#include "SctpSocketHandler.h"

// Timeout (ms) for statistics
#define STAT_TIME 1000
#define RESTART_TIME 3000
#define MAX_COUNT 10

DrtaMW2 ::DrtaMW2( QWidget* parent, const char* name, WFlags fl )
    : DrtaMW( parent, name, fl ), config(Config::instance())
{
#ifdef REN_DEBUG
	qWarning("DrtaMW2::DrtaMW2()");
#endif
	setName( "DRTA" );
	setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, sizePolicy().hasHeightForWidth() ) );
	setMinimumSize(QSize(300, 400));	// renyang - 設定此widget的最小長寬
	// renyang - minimumSizeHint()回傳目前這一個視窗的建議的最小長寬
	resize( QSize(300, 400).expandedTo(minimumSizeHint()) );
	
	statusbar = statusBar();
	QWidget *trafficWidget = new QWidget( this, "trafficWidget");
	QBoxLayout *tLayout = new QHBoxLayout(trafficWidget);
	trafficLabel = new QLabel( trafficWidget, "trafficLabel" );
	tLayout->addWidget(trafficLabel);
	statusbar->addWidget( trafficWidget, 0, TRUE);
	
	lockWidget = new QWidget( this, "lockWidget");
	QBoxLayout *lLayout = new QHBoxLayout(lockWidget);
	lockPixmap = new QLabel( lockWidget, "txPixmap" );
	lockPixmap->setPixmap( QPixmap::fromMimeSource( "lock.png" ) );
	lockPixmap->setEnabled( FALSE );
	lLayout->addWidget(lockPixmap);
	statusbar->addWidget( lockWidget, 0, TRUE);

/* set Icons */
	rxPixmap->setPixmap( QPixmap::fromMimeSource( "green.png" ) );
	rxPixmap->setEnabled(FALSE);
	txPixmap->setPixmap( QPixmap::fromMimeSource( "red.png" ) );
	txPixmap->setEnabled(FALSE);

	waitButton->setIconSet( QIconSet( QPixmap::fromMimeSource( "receive_no.png" ) ) );
	muteMicButton->setIconSet( QIconSet( QPixmap::fromMimeSource( "mic.png" ) ) );
	settingsButton->setIconSet( QIconSet( QPixmap::fromMimeSource( "configure.png" ) ) );
	muteSpkButton->setIconSet( QIconSet( QPixmap::fromMimeSource( "speaker.png" ) ) );
	settingsAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "configure.png" ) ) );

	logViewer = new LogViewer(this);
	timer = new QTimer(this);
	restartTimer = new QTimer(this);
	
	peer_addr_lst . setAutoDelete(true);
	
	hostEdit->clear();
	for (int i=0; i<MAX_COUNT; i++)
	{
		QString host_name = config.readEntry(QString("/drta/host/%1").arg(i));
		if (!host_name.isEmpty())
			hostEdit->insertItem(host_name);
	}

	hostEdit->setEditText(config.readEntry("/drta/host/default"));
	thSlider->setValue(config.readNumEntry("/drta/sound/threshold"));
	agcSlider->setValue(config.readNumEntry("/drta/sound/agclevel"));
	
	if (config.readBoolEntry("/drta/sound/adr"))
		adrAction->toggle();
	
	if (config.readBoolEntry("/drta/sound/agc"))
		agcAction->toggle();

	connect( timer, SIGNAL(timeout()), this, SLOT(statistics()) );
	connect( restartTimer, SIGNAL(timeout()), this, SLOT(listen()));

	//signals and slots connections
	connect( settingsAction, SIGNAL( activated() ), this, SLOT( settings() ) );
	connect( cryptAction, SIGNAL( toggled(bool) ), this, SLOT( crypt(bool) ) );
	connect( changeKeyAction, SIGNAL( activated() ), this, SLOT( changeKey() ) );
	connect( setDecKeyAction, SIGNAL( activated() ), this, SLOT( setDecryptionKey() ) );
	connect( logButton, SIGNAL( clicked() ), this, SLOT( log() ) );
	connect( settingsButton, SIGNAL( clicked() ), this, SLOT( settings() ) );
	connect( ringButton, SIGNAL( toggled(bool) ), this, SLOT( ringOn(bool) ) );
	connect( callButton, SIGNAL( clicked() ), this, SLOT( startAll() ) );
	connect( stopButton, SIGNAL( clicked() ), this, SLOT( stop() ) );
	connect( waitButton, SIGNAL( toggled(bool) ), this, SLOT( waitCalls(bool) ) );
	connect( adrAction, SIGNAL( toggled(bool) ), this, SLOT( adrOn(bool) ) );
	connect( agcAction, SIGNAL( toggled(bool) ), this, SLOT( agcOn(bool) ) );
	connect( agcSlider, SIGNAL( valueChanged(int) ), this, SLOT( agcSliderChanged(int) ) );
	connect( thSlider, SIGNAL( valueChanged(int) ), this, SLOT( sliderChanged(int) ) );
	connect( thSlider, SIGNAL( sliderPressed() ), this, SLOT( sliderPress() ) );
	connect( thSlider, SIGNAL( sliderReleased() ), this, SLOT( sliderRelease() ) );
	connect( filePlayFileAction, SIGNAL( activated() ), this, SLOT( play() ) );
	connect( muteMicButton, SIGNAL( toggled(bool) ), this, SLOT( transmitterStatus(bool) ) );
	connect( muteSpkButton, SIGNAL( toggled(bool) ), this, SLOT( receiverStatus(bool) ) );
	//connect( hostEdit->lineEdit(), SIGNAL( returnPressed() ), this, SLOT( startAll() ) );
	connect( hostEdit->lineEdit(), SIGNAL( returnPressed() ), this, SLOT( slotIPAdd() ) );

	allow_auto_ho = config.readBoolEntry("/drta/sctp/allow_auto");
	error_threshold = config.readNumEntry("/drta/sctp/err_thr");

	hand_over_policy = config.readNumEntry("/drta/sctp/ho_policy");
	reliable_path = 0;
	/* TODO: remove this */
	languageChange();
	
}
void DrtaMW2::languageChange()
{
	setCaption( tr( "DRTA" ) );
	trafficLabel->setText( tr( "0.0 KB/s" ) );

	threshold->setText( QString( "Voice threshold: %1 \%" ).arg( thSlider->value() ) );
	agcLabel->setText( QString( "AGC volume: %1 \%" ).arg( agcSlider->value() ) );

	ringButton->setText( QString::null );
	helpContentsAction->setText( tr( "Contents" ) );
	helpContentsAction->setMenuText( tr( "&Contents..." ) );
	helpContentsAction->setAccel( QString::null );
	helpContentsAction->setStatusTip( tr( "Show help contents" ) );

	settingsAction->setText( tr( "Settings" ) );
	settingsAction->setMenuText( tr( "&Settings..." ) );
	settingsAction->setStatusTip( tr( "Adjust settings" ) );
	adrAction->setText( tr( "Audio Delay Reduction" ) );
	adrAction->setMenuText( tr( "Audio Delay &Reduction (ADR)" ) );
	adrAction->setStatusTip( tr( "Activates the automatic Audio Delay Reduction (ADR)" ) );
	agcAction->setText( tr( "Automatic Gain Control" ) );
	agcAction->setMenuText( tr( "Automatic &Gain Control (AGC)" ) );
	agcAction->setStatusTip( tr( "Activates the Automatic Gain Control (AGC)" ) );
	changeKeyAction->setText( tr( "Change encryption key..." ) );
	changeKeyAction->setMenuText( tr( "&Change encryption key" ) );
	changeKeyAction->setStatusTip( tr( "Change the encryption key" ) );
	setDecKeyAction->setText( tr( "Set/reset decryption passphrase..." ) );
	setDecKeyAction->setMenuText( tr( "Set/reset &decryption key" ) );
	setDecKeyAction->setStatusTip( tr( "Set/reset decryption key" ) );
	cryptAction->setText( tr( "Encrypt outgoing stream" ) );
	cryptAction->setMenuText( tr( "&Encrypt outgoing stream" ) );
	cryptAction->setStatusTip( tr( "Encrypt the outgoing stream" ) );
	
	muteMicButton->setAccel( QKeySequence( tr( "Alt+I" ) ) );
	muteSpkButton->setAccel( QKeySequence( tr( "Alt+O" ) ) );
	ringButton->setAccel( QKeySequence( tr( "Alt+B" ) ) );
	
	QToolTip::add( hostEdit, tr( "Write the address (IP or DNS hostname) of the computer that you want to call.\nThen press Enter or click the Call button to start the communication." ) );
	QToolTip::add( agcSlider, tr( "AGC volume level" ) );
	QToolTip::add( thSlider, tr( "Threshold value: set up to a value that\nTX led is off when you are not speaking" ) );
	QToolTip::add( tx, tr( "This led is on when transmitting data" ) );
	QToolTip::add( rx, tr( "This led is on when receiving data" ) );
	QToolTip::add( trafficLabel, tr( "Total network traffic" ) );
	QToolTip::add( lockPixmap, tr( "Encryption status" ) );
	QToolTip::add( muteMicButton, tr( "Disable audio input" ) );
	QToolTip::add( muteSpkButton, tr( "Disable audio output" ) );
	QToolTip::add( callButton, tr( "Start the communication" ) );
	QToolTip::add( stopButton, tr( "Hang up" ) );
	QToolTip::add( waitButton, tr( "Enable this button to receive calls" ) );
	QToolTip::add( ringButton, tr( "Ring!" ) );
	QToolTip::add( logButton, tr( "Open the Log Viewer" ) );
	QToolTip::add( settingsButton, tr( "Open the Settings dialog to configure DRTA" ) );
	
}

/*
 *  Destroys the object and frees any allocated resources
 */
DrtaMW2::~DrtaMW2()
{
	// no need to delete child widgets, Qt does it all for us
	delete rsa;
	delete transmitter;
	delete receiver;
	delete player;
	delete recorder;
	delete logger;
}


void DrtaMW2::initDrta(bool file)
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::initDrta(%1)").arg(file));
#endif
	show();	// renyang - 顯示目前這一個GUI,在Qt中所有GUI預設是隱藏的
	
	try
	{
		fromFile = file;
		sliderFree = true;
		listening = false;
		received = false;
		tocrypt = false;
		closing = false;
		
		rsa = new Rsa(RSA_STRENGTH);	// renyang - 設定加解密的強度
	
		player = new AudioPlayer();	// renyang - 這一個類別主要是語部編碼的部分
		receiver = new Receiver(player, rsa);	// renyang - 應該是在解語音的部分
		//receiver = new Receiver(player);	// renyang - 原本沒有加密的部分,最後有加上加密的部分
	
		recorder = new Recorder();	// renyang - 可能用來記錄語音編碼之後的資料
		transmitter = new Transmitter(recorder, rsa);	// renyang - 把編碼後的資料透過加密之後,產生傳送物件
		transmitter -> setTTL( config.readNumEntry("/drta/sctp/ttl") );	// 設定time to live
	
		logger = new Logger();
	
		connect( receiver, SIGNAL(newSocket(int,int,struct sockaddr_in)), this, SLOT(newConnection(int,int,struct sockaddr_in)) );
		connect( receiver, SIGNAL(keyRequest()), this, SLOT(sendKeyRequest()) );
		connect( receiver, SIGNAL(sendNewKey()), this, SLOT(sendKey()) );
		connect( receiver, SIGNAL(newKey(QString)), this, SLOT(receivedNewKey(QString)) );
		connect( receiver, SIGNAL(finish()), this, SLOT(stopSignal()) );
		connect( receiver, SIGNAL(message(QString)), this, SLOT(message(QString)) );
		connect( receiver, SIGNAL(warning(QString)), this, SLOT(warning(QString)) );
		connect( receiver, SIGNAL(error(QString)), this, SLOT(abortAll(QString)) );
		connect( receiver, SIGNAL(fileProgress(int)), this, SLOT(changeProgress(int)) );
		connect( receiver, SIGNAL(ledEnable(bool)), this, SLOT(rxLedEnable(bool)) );
		connect( receiver, SIGNAL(ringReply()), transmitter, SLOT(sendRingReplyPacket()) );
		connect( receiver, SIGNAL(sigRecvMsg(QString)), this, SLOT(slotRecvMsg(QString)) );	
		connect( receiver, SIGNAL(sigRecvFrom(QString)), this, SLOT(slotRecvFrom(QString)) );	
		connect( receiver, SIGNAL(sigSctpEvent(QString)), this, SLOT(slotSctpEvent(QString)) );	
		connect( receiver, SIGNAL(sigRemoteTerminate()), this, SLOT(slotRemoteTerminate()) );	
		connect( receiver, SIGNAL(sigPrSendFailed()), this, SLOT(slotPrSendFailed()) );	

		connect( transmitter, SIGNAL(finish()), this, SLOT(stopSignal()) );
		connect( transmitter, SIGNAL(message(QString)), this, SLOT(message(QString)) );
		connect( transmitter, SIGNAL(ringMessage()), this, SLOT(ringMessage()) );
		connect( transmitter, SIGNAL(ledEnable(bool)), this, SLOT(txLedEnable(bool)) );
		connect( transmitter, SIGNAL(error(QString)), this, SLOT(abortAll(QString)) );	
		connect( player, SIGNAL(warning(QString)), this, SLOT(showWarning(QString)) );	
		connect( recorder, SIGNAL(warning(QString)), this, SLOT(showWarning(QString)) );	
	
		applySettings();
	}
	catch (Error e)
	{
		showMessageCritical(e.getText());
		qWarning("Program abort due to a critical error:" + e.getText());
		exit(1);
	}
	
	if (config.readBoolEntry("/drta/general/autohide"))
		hide();
	
	if (config.readBoolEntry("/drta/security/crypt"))
		cryptAction->toggle();

	if (!fromFile && config.readBoolEntry("/drta/general/autowait"))
		waitForCalls();
}

void DrtaMW2::applySettings()
{
#ifdef REN_DEBUG
	qWarning("DrtaMW2::applySettings()");
#endif
	int srate, quality, vbr, abr, complexity, vad, dtx, txstop;
	float vbrquality;

	transmitter->dump(config.readEntry("/drta/general/name"), config.readEntry("/drta/general/dumpout"));
	receiver->dump(config.readEntry("/drta/general/dumpin"));
	
	switch (config.readNumEntry("/drta/speex/mode"))
	{
		case 0:
				srate = 8000;
				break;
		case 1:
				srate = 16000;
				break;
		case 2:
				srate = 32000;
				break;
		default:
				srate = 8000;
				break;
	}
	quality = config.readNumEntry("/drta/speex/quality");
	abr = config.readNumEntry("/drta/speex/abr")*1000;
	vbr = config.readNumEntry("/drta/speex/bitratemode");
	switch (vbr)
	{
		case 0:
				abr = 0;
				break;
		case 1:
				abr = 0;
				break;
		case 2:
				vbr = 0;
				break;
	}
	vbrquality = (float) config.readNumEntry("/drta/speex/vbrquality");
	complexity = config.readNumEntry("/drta/speex/complexity");
	vad = config.readBoolEntry("/drta/speex/vad");
	dtx = config.readBoolEntry("/drta/speex/dtx");
	txstop = config.readNumEntry("/drta/sound/txstop");
	
	int th = 0;
	if (dtx)
	{
		thSlider->setEnabled(TRUE);
		th = config.readNumEntry("/drta/sound/threshold");
	}
	else 
		thSlider->setEnabled(FALSE);
	transmitter->setThreshold(th);
	transmitter->setup(srate, quality, abr, vbr, vbrquality, complexity, vad, dtx, txstop);

	recorder->setup(config.readNumEntry("/drta/sound/inputdriver"), config.readEntry("/drta/sound/inputinterface"));
	player->setup(config.readNumEntry("/drta/sound/outputdriver"), config.readEntry("/drta/sound/outputinterface"), config.readNumEntry("/drta/sound/ringvolume"));
	
	if (fromFile || config.readNumEntry("/drta/sound/outputdriver"))
		adrAction->setEnabled(FALSE);
	else
	{
		adrAction->setEnabled(TRUE);
		adrRefresh(config.readBoolEntry("/drta/sound/adr"));
	}
	
	agcRefresh(config.readBoolEntry("/drta/sound/agc"));
	
	logger->enable(config.readEntry("/drta/security/logfile"));
}

void DrtaMW2::txLedEnable(bool on)
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::txLedEnable(%1)").arg(on));
#endif
	txPixmap->setEnabled(on);
}

void DrtaMW2::startAll()
{
#ifdef REN_DEBUG
	qWarning("DrtaMW2::startAll()");
#endif
	if (fromFile)
		receiver->swap();
	else
	{
		skipStat = 0;
		seconds = 0;
		if (received){
			answer();
	//		SctpRefreshRemoteIP( s );
		}else{
			switch( config.readNumEntry("/drta/net/protocol") )
			{
			
				case DRTA_UDP:
				case DRTA_TCP:
					debug("use tranditional call");
					call(hostEdit->currentText());
					break;
				case DRTA_SCTP:
				case DRTA_SCTP_UDP:
					debug("use connex call");
					Callx();
					break;
				default:
					// never goto this line
					break;

			}
		}
	}
}

void DrtaMW2::answer()
{
#ifdef REN_DEBUG
	qWarning("DrtaMW2::answer()");
#endif
	try
	{
		UiStateChange( UI_ANSWER );

		received = false;
		warning(QString("Connected with %1 (%2).").arg(receiver->getCallerName()).arg(receiver->getIp()));
		receiver->resetCalls();
		transmitter->answer();
		timer->start(STAT_TIME, false);
	}
	catch (Error e)
	{
		abortAll(e.getText());
	}
}

void DrtaMW2::call(QString host)
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::call(%1)").arg(host));
#endif
	try
	{
		if (host.isEmpty())
			throw Error(tr("No host specified!"));

			UiStateChange( UI_CALL );

		if (listening)
		{
			listening = false;
			receiver->end();
		}
		int sd =  transmitter->call(host, config.readNumEntry("/drta/net/outport"), config.readNumEntry("/drta/net/protocol"));
		receiver->start(sd, config.readNumEntry("/drta/net/protocol"));
		logViewer->addLog(logger->logOutgoingCall(host, transmitter->getIp()));

			/* TODO: FSM-STATE */
		ringButton->toggle();
			/* TODO: FSM-STATE */

		receiver->waitConnection();
		if (hostEdit->listBox()->findItem(host) == 0)
			hostEdit->insertItem(host, 0);
	}
	catch (Error e)
	{
		abortAll(e.getText());
	}
}

void DrtaMW2::Callx()
{
#ifdef REN_DEBUG
	qWarning("DrtaMW2::Callx()");
#endif
	QStrList host_list;
	unsigned int i = 0;

	try
	{
		if ( !hostEdit->currentText().isEmpty() ){
			lb_ips -> insertItem( hostEdit->currentText() );
			hostEdit->clear();
		}
		// prepare ip list
		for( i = 0 ; i < lb_ips -> count() ; i++ ){
			host_list . append( lb_ips -> text(i) );
		}


		if (listening)
		{
			listening = false;
			receiver->end();
		}
		int sd =  transmitter->Callx(host_list
					,config.readNumEntry("/drta/net/outport")
					,config.readNumEntry("/drta/net/protocol"));
		receiver->start(sd, config.readNumEntry("/drta/net/protocol"));

		SctpRefreshRemoteIP( sd );

		// TODO:
		//logViewer->addLog(logger->logOutgoingCall(host, transmitter->getIp()));

/*
		QString s ;
		s.sprintf("rto_max = %d , rto_min =%d ",SctpSocketHandler::SctpGetRtoMax(sd) ,SctpSocketHandler::SctpGetRtoMin(sd) ) ;
		logViewer->addLog(logger->logOutgoingCall("SctpSocket", s) ); 
		*/
		SctpSocketHandler::SctpSetRtoMax(sd , config.readNumEntry("/drta/sctp/rto_max"));
		SctpSocketHandler::SctpSetRtoMin(sd , config.readNumEntry("/drta/sctp/rto_min")); 
		
		ringButton->toggle();
		UiStateChange( UI_CALL );

		receiver->waitConnection();
	}
	catch (Error e)
	{
		abortAll(e.getText());
	}
}

void DrtaMW2::waitCalls(bool on)
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::waitCalls(%1)").arg(on));
#endif
	if (on)
	{
		UiStateChange( UI_WAITCALL_RECV );
		receiver->resetCalls();
		listen();
	}
	else
	{
		UiStateChange( UI_WAITCALL_RECV_NO );

		logViewer->addLog(logger->logStopReceiver(receiver->getCalls(), receiver->getConnections()));
		message(tr("Receiver stopped."));
		receiver->end();
		listening = false;
	}
}

void DrtaMW2::listen()
{
#ifdef REN_DEBUG
	qWarning("DrtaMW2::listen()");
#endif
	try {
		filePlayFileAction->setEnabled(FALSE);

		message("Starting Receiver...");
		listening = true;
		// renyang - 取回連線數
		if (receiver->getConnections() > 0)
		{
			rxLedEnable(TRUE);
		}
		receiver->Listen(
			config.readNumEntry("/drta/net/inport"), 
			config.readBoolEntry("/drta/net/udp"), 
			config.readBoolEntry("/drta/net/tcp") ,
			config.readBoolEntry("/drta/net/sctp"),
			config.readBoolEntry("/drta/net/sctp_udp")
			);
	}
	catch (Error e)
	{
		abortAll(e.getText());
	}
}

// renyang - 表示client端有資料送過來啦
void DrtaMW2::newConnection(int sd, int protocol, struct sockaddr_in sa)
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::newConnection(%1,%2,struct sockaddr_in sa)").arg(sd).arg(protocol));
#endif
	try
	{
		if (listening)
		{
//			callButton->setText( tr("&Answer") );
			callButton->setEnabled(TRUE);
//			stopButton->setText( tr("&Refuse") );
			stopButton->setEnabled(TRUE);
			waitButton->setEnabled(FALSE);
			tx->setEnabled(TRUE);
			rx->setEnabled(TRUE);
			received = true;
			// renyang - 設定Transmitter的protocol
			transmitter->newConnection(sd, sa, protocol);
			if (config.readBoolEntry("/drta/general/answer"))
				answer();

			SctpRefreshRemoteIP( sd );
		}
		else
		{
			if (ringButton->isOn())
				ringButton->toggle();
			timer->start(STAT_TIME, false);
		}
	}
	catch (Error e)
	{
		abortAll(e.getText());
	}
}

void DrtaMW2::stopSignal()
{
#ifdef REN_DEBUG
	qWarning("DrtaMW2::stopSignal()");
#endif
	if (fromFile)
	{
		statusbar->message(tr("End of file"));
	}
	else
	{
		QString text = QString("%1 ").arg(receiver->getCallerName());
		if (receiver->refused())
			text += tr("refused the call.");
		else
		if (receiver->getTotal() > 0)
			text += tr("closed the call.");
		else
			text = QString("%1 rejected the call.").arg(receiver->getIp());
		logViewer->addLog(logger->logStop(receiver->getTotal(), transmitter->getTotal()));
		warning(text);
	}
	stopAll();
}

void DrtaMW2::stop()
{
#ifdef REN_DEBUG
	qWarning("DrtaMW2::stop()");
#endif
	if (received)
		transmitter->sendRefusePacket();
	else
		transmitter->sendClosePacket();
	if (fromFile)
	{
		message("Stopped.");
	}
	else
	{
		message("Communication closed.");
		logViewer->addLog(logger->logStop(receiver->getTotal(), transmitter->getTotal()));
	}
	stopAll();
}

void DrtaMW2::stopAll()
{
#ifdef REN_DEBUG
	qWarning("DrtaMW2::stopAll()");
#endif
	raise();
	try
	{
		UiStateChange( UI_STOPALL );

		timer->stop();
		fileName = QString::null;
		if (fromFile)
		{
			fromFile = false;
			adrAction->setEnabled(TRUE);
			adrRefresh(config.readBoolEntry("/drta/sound/adr"));
			thSlider->setValue(config.readNumEntry("/drta/sound/threshold"));
		}
		received = false;
		receiver->end();
		transmitter->end();
		sliderFree = true;
		rxLedEnable(FALSE);
		txLedEnable(FALSE);
		// renyang - 當停止所有的call後, 若waitButton仍是被按住的話, 則執行listen()
		if (waitButton->isOn())
			restartTimer->start(RESTART_TIME, true);
	}
	catch (Error e)
	{
		QMessageBox::critical(0, "DRTA Error", tr("Error: ") + e.getText());
	}
}

void DrtaMW2::play()
{
#ifdef REN_DEBUG
	qWarning("DrtaMW2::play()");
#endif
        QString name=QFileDialog::getOpenFileName("","*"DRTA_EXT,this,0,"Open DRTA file to play...");
	playFile(name);
}

void DrtaMW2::playFile(QString name)
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::playFile(%1)").arg(name));
#endif
	if (!name.isEmpty())
	{
		try
		{
			sliderFree = true;
			
			UiStateChange( UI_PLAY_FILE);

			message("Playing " + fileName + "...") ;
			fileName = name;
			fromFile = true;
			receiver->playFile(fileName);
			timer->start(STAT_TIME, false);
		}
		catch (Error e)
		{
			abortAll(e.getText());
		}
	}
}

void DrtaMW2::abortAll(QString message)
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::abortAll(%1)").arg(message));
#endif
	if (waitButton->isOn())
		waitButton->toggle();
	stopAll();
	showMessageCritical(message);
}

void DrtaMW2::showMessageCritical(QString text)
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::showMessageCritical(%1)").arg(text));
#endif
	message("Aborted");
	QMessageBox::critical(0, "DRTA Error", tr("Error: ") + text);
}

void DrtaMW2::showWarning(QString text)
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::showWarning(%1)").arg(text));
#endif
	message("WARNING!");
	QMessageBox::warning(0, "DRTA Warning", tr("Warning: ") + text);
}

void DrtaMW2::closeEvent(QCloseEvent *e)
{
#ifdef REN_DEBUG
	qWarning("DrtaMW2::closeEvent()");
#endif
	if (closing)
	{
		e->accept();
	}
	else
	{
		e->ignore();
		hide();
	}
}

void DrtaMW2::fileExit()
{
#ifdef REN_DEBUG
	qWarning("DrtaMW2::fileExit()");
#endif
	stopAll();
	closing = true;
	config.writeEntry("/drta/host/default", hostEdit->currentText());
	for (int i=0; (i<hostEdit->count()) && (i<MAX_COUNT); i++)
	{
		config.writeEntry(QString("/drta/host/%1").arg(i), hostEdit->text(i));
	}
	close();
}

void DrtaMW2::message(QString text)
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::message(%1)").arg(text));
#endif
	skipStat = 1;
	statusbar->message(text, STAT_TIME*3);
}

void DrtaMW2::warning(QString text)
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::warning(%1)").arg(text));
#endif
	skipStat = 2;
	statusbar->message(text);

	logViewer->addLog(logger->log(text));
	raise();
}

void DrtaMW2::log()
{
#ifdef REN_DEBUG
	qWarning("DrtaMW2::log()");
#endif
	logViewer->show();
}

void DrtaMW2::settings()
{
#ifdef REN_DEBUG
	qWarning("DrtaMW2::settings()");
#endif
	Settings *settings = new Settings(this);
	int ret = settings->exec();
	if (ret == QDialog::Accepted)
	{
		try
		{
			applySettings();
		}
		catch (Error e)
		{
			abortAll(e.getText());
		}
	}
	delete settings;
}

void DrtaMW2::ringOn(bool on)
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::ringOn(%1)").arg(on));
#endif
	transmitter->ring(on);
}

void DrtaMW2::cryptOn()
{
#ifdef REN_DEBUG
	qWarning("DrtaMW2::cryptOn()");
#endif
	if (!cryptAction->isOn())
		cryptAction->toggle();
}

void DrtaMW2::crypt(bool on)
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::crypt(%1)").arg(on));
#endif
	config.writeEntry("/drta/security/crypt", on);
	if (on)
	{
		srand(time(NULL));
		if (changeKey())
		{
			changeKeyAction->setEnabled(TRUE);
			lockPixmap->setEnabled(TRUE);
			message(tr("Encryption enabled."));
		}
		else
			cryptAction->toggle();
	}
	else
	{
		lockPixmap->setEnabled(FALSE);
		message(tr("Encryption disabled."));
		transmitter->disableCrypt();
		changeKeyAction->setEnabled(FALSE);
	}
}

bool DrtaMW2::changeKey()
{
#ifdef REN_DEBUG
	qWarning("DrtaMW2::changeKey()");
#endif
	bool ok = false;
	try
	{
		if (config.readBoolEntry("/drta/security/random"))
		{
			int i, len = config.readNumEntry("/drta/security/keylen")/8;
			char key[len];
			for (i=0; i<len; i++)
				key[i] = (char)((rand()%256)-128);
			transmitter->enableCrypt(key, len);
			ok = true;
		}
		else
		{
			QString text = QInputDialog::getText("Encryption passphrase change", "Enter the encryption passphrase (max 56 chars, please use at least 30 chars):", QLineEdit::Password, QString::null, &ok, this );
			if (ok)
			{
				ok = false;
				if (!text.isEmpty())
				{
					transmitter->enableCrypt((char *) text.ascii(), text.length());
					ok = true;
				}
			}
		}
		if (ok)
			message(tr("Encryption key changed successfully."));
	}
	catch (Error e)
	{
		abortAll(e.getText());
	}
	return ok;
}

void DrtaMW2::setDecryptionKey()
{
#ifdef REN_DEBUG
	qWarning("DrtaMW2::setDecryptionKey()");
#endif
	bool ok;
	QString text = QInputDialog::getText("Decryption Passphrase", "Enter the decryption passphrase, leave blank to reset.", QLineEdit::Password, QString::null, &ok, this );
	if (ok)
	{
		receiver->disableDecrypt();
		if (!text.isEmpty())
			receiver->enableDecrypt((char *) text.ascii(), text.length());
		message(tr("Decryption key changed successfully."));
	}
}

void DrtaMW2::sendKey()
{
#ifdef REN_DEBUG
	qWarning("DrtaMW2::sendKey()");
#endif
	if (transmitter->isWorking())
		transmitter->sendKeyPacket();
}

void DrtaMW2::sendKeyRequest()
{
#ifdef REN_DEBUG
	qWarning("DrtaMW2::sendKeyRequest()");
#endif
	if (transmitter->isWorking())
		transmitter->sendKeyRequestPacket();
	else
	{
		receiver->stop();
		switch (QMessageBox::warning(0, "DRTA Error", "Warning: stream is crypted but the decryption key is not available.\n\nOK if you want to enter the decryption passphrase, IGNORE if you\nwant to continue anyway, CANCEL to stop.", QMessageBox::Ok, QMessageBox::Ignore, QMessageBox::Cancel))
		{
			case QMessageBox::Ok:
				setDecryptionKey();
				receiver->go();
				break;
			case QMessageBox::Ignore:
				receiver->noDecrypt();
				receiver->go();
				break;
			default:
				stopSignal();
				break;
		}
	}
}

void DrtaMW2::agcRefresh(bool on)
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::agcRefresh(%1)").arg(on));
#endif
	float step = (float) (config.readNumEntry("/drta/sound/agcstep")/1000.f);
	float level = (float) (config.readNumEntry("/drta/sound/agclevel")/100.f);
	float min = (float) config.readNumEntry("/drta/sound/agcmin");
	float max = (float) config.readNumEntry("/drta/sound/agcmax");
	player->setAgc(on, step, level, min, max);
	config.writeEntry("/drta/sound/agc", on);
	if (on)
	{
		agcSlider->setEnabled(TRUE);
		agcLabel->setEnabled(TRUE);
	}
	else
	{
		agcSlider->setEnabled(FALSE);
		agcLabel->setEnabled(FALSE);
	}
}

void DrtaMW2::agcOn(bool on)
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::agcOn(%1)").arg(on));
#endif
	agcRefresh(on);
	if (on)
		message(tr("Automatic Gain Control (AGC) enabled."));
	else
		message(tr("Automatic Gain Control (AGC) disabled."));
}

void DrtaMW2::adrRefresh(bool on)
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::adrRefresh(%1)").arg(on));
#endif
	float mindelay = (float) (config.readNumEntry("/drta/sound/adrmindelay")/1000.f);
	float maxdelay = (float) (config.readNumEntry("/drta/sound/adrmaxdelay")/1000.f);
	float stretch = (float) config.readNumEntry("/drta/sound/adrstretch");
	player->setAdr(on, mindelay, maxdelay, stretch);
	config.writeEntry("/drta/sound/adr", on);
}

void DrtaMW2::adrOn(bool on)
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::adrOn(%1)").arg(on));
#endif
	adrRefresh(on);
	if (on)
		message(tr("Audio Delay Reduction (ADR) enabled."));
	else
		message(tr("Audio Delay Reduction (ADR) disabled."));
}

void DrtaMW2::transmitterStatus(bool on)
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::transmitterStatus(%1)").arg(on));
#endif
	try
	{
		if (on)
		{
			message("Audio input mute.");
			transmitter->changeStatus(Transmitter::TRANSMITTER_STATUS_MUTE);
			muteMicButton->setIconSet( QIconSet( QPixmap::fromMimeSource( "mic_mute.png" ) ) );
			//muteMicButton->setText( tr("Mute") );
		}
		else 
		{
			message("Audio input enabled.");
			transmitter->changeStatus(Transmitter::TRANSMITTER_STATUS_WAITING);
			muteMicButton->setIconSet( QIconSet( QPixmap::fromMimeSource( "mic.png" ) ) );
			//muteMicButton->setText( tr("Enable") );
		}
	}
	catch (Error e)
	{
		abortAll(e.getText());
	}
}

void DrtaMW2::receiverStatus(bool on)
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::receiverStatus(%1)").arg(on));
#endif
	try
	{
		if (on)
		{
			message("Audio output mute.");
			receiver->changeStatus(Receiver::RECEIVER_STATUS_MUTE);
			muteSpkButton->setIconSet( QIconSet( QPixmap::fromMimeSource( "speaker_mute.png" ) ) );
			//muteSpkButton->setText( tr("Mute!") );
		}
		else 
		{
			message("Audio output enabled.");
			receiver->changeStatus(Receiver::RECEIVER_STATUS_NORMAL);
			muteSpkButton->setIconSet( QIconSet( QPixmap::fromMimeSource( "speaker.png" ) ) );
			//muteSpkButton->setText( tr("Enable!") );
		}
	}
	catch (Error e)
	{
		abortAll(e.getText());
	}
}

void DrtaMW2::changeProgress(int v)
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::changeProgress(%1)").arg(v));
#endif
	if (sliderFree)
		thSlider->setValue(v);
}

void DrtaMW2::sliderChanged(int v)
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::sliderChanged(%1)").arg(v));
#endif
	if (fromFile)
	{
		threshold->setText(QString("File progress: %1 \%").arg( v ));
	}
	else
	{
		transmitter->setThreshold(v);
		threshold->setText(QString("Voice threshold: %1 \%").arg( v ));
		config.writeEntry("/drta/sound/threshold", v);
	}
}

void DrtaMW2::sliderPress()
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::sliderPress()"));
#endif
	if (fromFile)
	{
		receiver->stop();
		sliderFree = false;
	}
}

void DrtaMW2::sliderRelease()
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::sliderRelease()"));
#endif
	if (fromFile)
	{
		receiver->seekFile(thSlider->value());
		receiver->go();
		sliderFree = true;
	}
}

void DrtaMW2::agcSliderChanged(int v)
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::agcSliderChanged(%1)").arg(v));
#endif
	bool agc = config.readBoolEntry("/drta/sound/agc");
	config.writeEntry("/drta/sound/agclevel", v);
	agcLabel->setText(QString("AGC volume: %1 \%").arg( v ));
	agcRefresh(agc);
}

void DrtaMW2::statistics()
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::statistics()"));
#endif
	if (fromFile)
	{
		message(QString("Playing %1 (%2)").arg(fileName).arg(receiver->getCallerName()));
	}
	else
	{
		seconds++;
		float tot = (float) STAT_TIME;
		float tx_kb = (float) transmitter->getBytes()/tot;
		float rx_kb = (float) receiver->getBytes()/tot;
		QToolTip::add( txPixmap, QString( "%1 KB/s" ).arg(tx_kb, 2, 'f', 1 ) );
		QToolTip::add( rxPixmap, QString( "%1 KB/s" ).arg(rx_kb, 2, 'f', 1 ) );
		tot = rx_kb + tx_kb;
		QString statName = QString("%1").arg(receiver->getCallerName());
		QString statTime;
		statTime.sprintf("%02d:%02d", (int)(seconds/60), (int)(seconds%60));
		QString statTraffic = QString("%1 KB/s").arg(tot, 2, 'f', 1 );
		trafficLabel->setText(statTraffic);
		if (skipStat > 0)
			skipStat--;
		else
			statusbar->message(QString ("%1 - %2").arg(statName).arg(statTime));
		
		if (tocrypt)
		{
			cryptOn();
			tocrypt = false;
		}
	}
}

void DrtaMW2::helpContents()
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::helpContents()"));
#endif
	QMessageBox::information( this, "Dr.ta" , "" );
}

void DrtaMW2::helpAbout()
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::helpAbout()"));
#endif
	QMessageBox::about( this, "Dr.ta help" ,"<center><b>Dr.ta</b> means <b>\"Driving Talking\"</b>.</center><br>"
				         "Dr.ta is a implementation program, <br>"
					 "based on sctp protocol and IHU,of MOD.<br>"
					 "It integrates the voice/text and <br>"
					 "supports multi-network communication.<br><br>"
					 "<center>http://mod.0rz.net/</center><br>"
					 "<center>http://drta.0RZ.NET/</center><BR>"
					 );
}

void DrtaMW2::disableIn()
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::disableIn()"));
#endif
	muteMicButton->toggle();
}

void DrtaMW2::disableOut()
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::disableOut()"));
#endif
	muteSpkButton->toggle();
}

void DrtaMW2::waitForCalls()
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::waitForCalls()"));
#endif
	if (!waitButton->isOn())
		waitButton->toggle();
}

void DrtaMW2::ringMessage()
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::ringMessage()"));
#endif
	QString text = QString("Ringing %1").arg(transmitter->getIp());
	if (receiver->replied())
		text += QString(" (%1)").arg(receiver->getCallerName());
	else
		text += QString(" (waiting for reply...)");
	warning(text);
}

void DrtaMW2::toggleVisibility()
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::toggleVisibility()"));
#endif
	if (isVisible())
		hide();
	else
		show();
}

void DrtaMW2::receivedNewKey(QString text)
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::receivedNewKey(%1)").arg(text));
#endif
	if (config.readBoolEntry("/drta/security/showkey"))
	{
		warning(QString("New decryption key: %1").arg(text));
	}
	tocrypt = true;
}
void DrtaMW2::UiStateChange( Ui_State now_s)
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::UiStateChange()"));
#endif

	switch ( now_s){

		case UI_STOPALL:
			callButton->setEnabled(TRUE);
		//	callButton->setText("&Call");
			stopButton->setEnabled(FALSE);
		//	stopButton->setText("&Hang up");
			hostEdit->setEnabled(TRUE);
			hostname->setEnabled(TRUE);
		//	filePlayFileAction->setEnabled(TRUE);
			ringButton->setEnabled(FALSE);
			waitButton->setEnabled(TRUE);
			tx->setEnabled(FALSE);
			rx->setEnabled(FALSE);
			trafficLabel->setText( tr( "0.0 KB/s" ) );
			QToolTip::add( txPixmap, QString::null);
			QToolTip::add( rxPixmap, QString::null);
			if (ringButton->isOn())
				ringButton->toggle();

			msg_frame -> setEnabled(FALSE);	
			break;
		case UI_CALL:
			callButton->setEnabled(TRUE);
			callButton->setEnabled(FALSE);
			stopButton->setEnabled(TRUE);
//			stopButton->setText("&Hang up");
			hostEdit->setEnabled(FALSE);
			hostname->setEnabled(FALSE);
			filePlayFileAction->setEnabled(FALSE);
			ringButton->setEnabled(TRUE);
			waitButton->setEnabled(FALSE);
			tx->setEnabled(TRUE);
			rx->setEnabled(TRUE);
			restartTimer->stop();

			pb_sendmsg -> setEnabled(TRUE);
			te_sendmsg -> setEnabled(TRUE);
			msg_frame -> setEnabled(TRUE);	

			ringButton->toggle();
			break;
		case  UI_WAITCALL_RECV_NO :
			restartTimer->stop();
		//	filePlayFileAction->setEnabled(TRUE);
			rxLedEnable(FALSE);
			waitButton->setIconSet( QIconSet( QPixmap::fromMimeSource( "receive_no.png" ) ) );

			msg_frame -> setEnabled(FALSE);	
			break;

		case  UI_WAITCALL_RECV:
			waitButton->setIconSet( QIconSet( QPixmap::fromMimeSource( "receive.png" ) ) );

			msg_frame -> setEnabled(FALSE);	
			break;

		case UI_PLAY_FILE:
			stopButton->setEnabled(TRUE);
			waitButton->setEnabled(FALSE);
			filePlayFileAction->setEnabled(FALSE);
			adrAction->setEnabled(FALSE);
			hostEdit->setEnabled(FALSE);
			hostname->setEnabled(FALSE);
/*			callButton->setText( tr("&Pause") );
			stopButton->setText( tr("S&top") );*/

			msg_frame -> setEnabled(FALSE);	
			break;

		case UI_ANSWER:
			callButton->setEnabled(FALSE);
			stopButton->setEnabled(TRUE);
//			stopButton->setText("&Hang up");
			hostEdit->setEnabled(FALSE);
			hostname->setEnabled(FALSE);
			filePlayFileAction->setEnabled(FALSE);
			ringButton->setEnabled(TRUE);
			waitButton->setEnabled(FALSE);

			pb_sendmsg -> setEnabled(TRUE);
			te_sendmsg -> setEnabled(TRUE);
			msg_frame -> setEnabled(TRUE);	
			break;

		default:
			debug("should no go to this line -> !");
			break;
	}
	
	return; 
}


void DrtaMW2 :: slotSendMsg()
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2 :: slotSendMsg()"));
#endif

	transmitter -> sendMsgPacket( te_sendmsg -> text() . utf8()  );

	QString to_append = " me  : " + te_sendmsg -> text();
	te_recvmsg -> append( to_append );

	te_sendmsg -> clear();
}

void DrtaMW2 :: slotRecvMsg(QString s)
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2 :: slotRecvMsg(%1)").arg(s));
#endif
	QString to_append = receiver->getCallerName() + " :" + s;
	te_recvmsg -> append( to_append );
}

void DrtaMW2::rxLedEnable(bool on)
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::rxLedEnable(%1)").arg(on));
#endif
	unsigned int i = 0;

	rxPixmap->setEnabled(on);

	if( on == false ){
		
		int cur = lb_recv_mtr -> currentItem();
		for( i = 0 ; i < lb_recv_mtr -> count() ; i++ ){
			if(peer_addr_lst . at(i) -> getRecv()  ){
				lb_recv_mtr -> changeItem( QPixmap::fromMimeSource( "little_blue.png" ) 
					, lb_recv_mtr -> item( i ) -> text() , i  );
				peer_addr_lst . at(i) -> setRecv(false);
			}
		} 
		lb_recv_mtr -> setCurrentItem(cur);
	}

}


void DrtaMW2::slotRemoteTerminate()
{
#ifdef REN_DEBUG
	qWarning(QString("DrtaMW2::slotRemoteTerminate()"));
#endif
	message(tr("Remote User Just Hang up!"));
	QMessageBox::warning(0, "DRTA MSG", tr("Remote User Just Hange up ") );
	
}

