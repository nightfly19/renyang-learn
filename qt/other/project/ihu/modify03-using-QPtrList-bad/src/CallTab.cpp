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
	// renyang - 設定這一個widget的佈局
	setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, sizePolicy().hasHeightForWidth() ) );

	// renyang - 設定這一個CallTab對方打電話過來的名字
	callName = QString(name);

	QBoxLayout *mainLayout = new QVBoxLayout( this, 0, 10,"mainLayout");

	mainFrame = new QFrame( this, "ledFrame" );
	mainLayout->addWidget(mainFrame);
	
	QBoxLayout *mLayout = new QVBoxLayout( mainFrame, 0, 10,"mLayout");

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

// renyang - 增加一個hostlist可以顯示所有的peer address-----------------------
	QBoxLayout *mhLayout0 = new QHBoxLayout(mLayout);

	mhLayout0->addSpacing(5);

	QBoxLayout *mvLayout = new QVBoxLayout(mhLayout0);

	hostEdit = new QComboBox( TRUE, mainFrame, "hostEdit" );
	hostEdit->setMinimumSize( QSize(100, 30) );
	// renyang - 設定自動補齊
	hostEdit->setAutoCompletion(TRUE);
	hostEdit->setDuplicatesEnabled(FALSE);
	// renyang - the string will not be inserted into the combobox
	hostEdit->setInsertionPolicy(QComboBox::NoInsertion);
	hostEdit->clear();

	hostList = new QListBox(mainFrame,"hostList");
	hostList->setMinimumSize(QSize(100,100));

	video_label = new QLabel(mainFrame,"video label");
	video_label->setMinimumSize(QSize(160,120));
	video_label->setMaximumSize(QSize(160,120));
	video_label->setPalette(QPalette( QColor( 250, 250, 200) ));

	video_check = new QCheckBox(mainFrame,"video_check");
	video_check->setEnabled(false);

	primButton = new QPushButton(mainFrame,"primButton");
	primButton->setMinimumSize(QSize(130,35));

	mvLayout->addSpacing(5);
	mvLayout->addWidget(hostEdit);
	mvLayout->addSpacing(5);

	QBoxLayout *mhLayout3 = new QHBoxLayout(mvLayout);

	mhLayout3->addWidget(hostList);
	mhLayout3->addSpacing(5);

	mhLayout3->addWidget(video_label);
	mhLayout3->addSpacing(5);

	mhLayout3->addWidget(video_check);
	mhLayout3->addSpacing(5);

	mvLayout->addSpacing(5);
	mvLayout->addWidget(primButton);
	mvLayout->addSpacing(5);

	mhLayout0->addSpacing(5);
// renyang --------------------------------------------------------------------
	
	mLayout->addStretch();
	
	QBoxLayout *mhLayout1 = new QHBoxLayout(mLayout, 25);
	
	mhLayout1->addSpacing(5);

	callButton = new QPushButton( mainFrame, "callButton" );
	callButton->setMinimumSize( QSize(130, 35) );
	callButton->setIconSet( QIconSet( QPixmap::fromMimeSource( "call.png" ) ) );
	mhLayout1->addWidget(callButton, 2);
	
	ringButton = new QToolButton( mainFrame, "ringButton" );
	ringButton->setFixedSize( QSize(40, 35) );
	// renyang - 設定為可以on/off的按扭
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

	// renyang - The QStatusBar class provides a horizontal bar suitable for presenting status information
	statusbar = new QStatusBar(this, "statusbar");
	// renyang - 讓QStatusBar的右下角不會出現可以調整大小的三角型
	statusbar->setSizeGripEnabled(FALSE);

	QWidget *trafficWidget = new QWidget( this, "trafficWidget");
	QBoxLayout *tLayout = new QHBoxLayout(trafficWidget);
	trafficLabel = new QLabel( trafficWidget, "trafficLabel" );
	trafficLabel->setEnabled(FALSE);
	tLayout->addWidget(trafficLabel);
	statusbar->addWidget( trafficWidget, 0, true);
	
	mainLayout->addWidget(statusbar);

	connect( ringButton, SIGNAL( clicked() ), this, SLOT( ringButtonClicked() ) );
	connect( callButton, SIGNAL( clicked() ), this, SLOT( callButtonClicked() ) );
	connect( stopButton, SIGNAL( clicked() ), this, SLOT( stopButtonClicked() ) );
	connect( hostEdit->lineEdit(), SIGNAL( returnPressed() ), this, SLOT( callButtonClicked() ) );
	connect( muteMicButton, SIGNAL( clicked() ), this, SLOT( muteMicButtonClicked() ) );
	connect( muteSpkButton, SIGNAL( clicked() ), this, SLOT( muteSpkButtonClicked() ) );
	// renyang-modify - 新增當按下primButton時, 會做什麼事
	connect( primButton,SIGNAL(clicked()),this,SLOT(primButtonClicked()));
	// renyang-modify - 當按下video_check扭時, 要求對方傳送image過來
	connect( video_check,SIGNAL(clicked()),this,SLOT(videoCheckChanged()));

	languageChange();

	callId = id;
	received = false;
	talking = false;
	seconds = 0;
	skipStat = 0;
	// renyang-modify - 初始化error_handled
	error_handled = 0;
	// renyang-modify - 初始化waiting, 當要向對方要求影像, 但是, 像還沒有整個上傳到CallTab時, waiting是true
	waiting = false;
	// renyang-modify - 初始化video_timer
	video_timer = new QTimer(this,"video_timer");
	// renyang-modify - 當時間到時要要求下一個frame
	connect(video_timer,SIGNAL(timeout()),this,SLOT(video_timeout()));
	// renyang-modify - 記錄對方的video是否Fail
	peervideofail = false;

	// renyang-modify - 暫時預設error_threshold為5
	error_threshold = 5;

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
	// renyang-modify - 加入按扭文字
	primButton->setText( tr ("Set &Primary Path"));
	// renyang-modify - end
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
#ifdef REN_DEBUG
	qWarning("CallTab::answer()");
#endif
	emit answerSignal(callId);
}

void CallTab::callButtonClicked()
{
#ifdef REN_DEBUG
	qWarning("CallTab::callButtonClicked()");
#endif
	if (received)
		answer();
	else
		call(hostEdit->currentText());
}

// renyang - 打電話給指定的host
void CallTab::call(QString host)
{
#ifdef REN_DEBUG
	qWarning(QString("CallTab::call(QString %1)").arg(host));
#endif
	// renyang - 若沒有在打過的電話簿中找到的話, 就安插進去
	if (hostEdit->listBox()->findItem(host) == 0)
		hostEdit->insertItem(host, 0);
	emit callSignal(callId, host);
}

// renyang - 當回答要接受電話時, 設定相關GUI的部分
void CallTab::startCall()
{
#ifdef REN_DEBUG
	qWarning("CallTab::startCall()");
#endif
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
#ifdef REN_DEBUG
	qWarning("CallTab::stopButtonClicked()");
#endif
	callButton->setEnabled(FALSE);
	stopButton->setEnabled(FALSE);
	ringButton->setEnabled(FALSE);

	// renyang-modify - 當停止通話時, 則不能要求對方的影像
	// video_check->setEnabled(FALSE);
	// video_check->setChecked(false);

	emit stopSignal(callId);
}

void CallTab::ringButtonClicked()
{
#ifdef REN_DEBUG
	qWarning("CallTab::ringButtonClicked()");
#endif
	bool on = false;
	if (ringButton->isOn())
		on = true;
	emit ringSignal(callId, on);
}

void CallTab::receivedCall(QString ip)
{
#ifdef REN_DEBUG
	qWarning(QString("CallTab::receivedCall(QString %1)").arg(ip));
#endif
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

// renyang - 本地端接受了遠端的通話，或是遠端接受了本地端的通話
void CallTab::connectedCall()
{
#ifdef REN_DEBUG
	qWarning("CallTab::connectedCall()");
#endif
	// renyang - 若本地端電話還在響, 則把它關掉
	if (ringButton->isOn())
		ringButton->setOn(false);
	// renyang-modify - 設定可以要求對方的影像
	video_check->setEnabled(TRUE);
	talking = true;
	// renyang - 預設一開始對方video是好的
	peervideofail = false;
}

// renyang - 當掛斷電話時, 或是對方掛斷電話時
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

	// renyang-modify - 清掉hostList
	hostList->clear();
	// renyang-modify - 不能要求對方的影像啦
	video_check->setEnabled(false);
	// renyang-modify - 結束通話, 清除video_check勾選
	video_check->setChecked(false);
	// renyang-modify - 清除video_label的所有資料
	clearVideoLabel();
	// renyang-modify - end
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
		// renyang - 取得開始通話的時間
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

void CallTab::change_hostList(QStringList addrs_list)
{
#ifdef REN_DEBUG
	qWarning("CallTab::change_hostList()");
#endif
	hostList->clear();
	for (QStringList::Iterator it = addrs_list.begin();it != addrs_list.end();++it)
	{
		hostList->insertItem(*it);
	}
	// renyang-modify - 預設都不能選
	for (unsigned int i=0;i<hostList->count();i++)
	{
		hostList->item(i)->setSelectable(false);
	}
}

void CallTab::primButtonClicked()
{
#ifdef REN_DEBUG
	qWarning("CallTab::primButtonClicked()");
#endif
	uint i;
	if (hostList->count()>0)
	{
		for (i=0;i<hostList->count();i++)
		{
			if (hostList->isSelected(i) && hostList->item(i)->isSelectable()) {
				// renyang - 設定其中一個為primary address
				qWarning(hostList->text(i));
				emit setPrimaddrSignal(callId,hostList->text(i));
				break;
			}
		}
	}
}

// renyang-modify - 這一個函式是用來改變ip list的狀態
void CallTab::setAddressEvent(QString ip,QString description)
{
#ifdef REN_DEBUG
	qWarning(QString("CallTab::AddressEvent(%1,%2)").arg(ip).arg(description));
#endif
	unsigned int i;
	for (i=0;i<hostList->count();i++)
	{
		if (hostList->text(i) == ip)
		{
			if (description == QString("ADDRESS CONFIRMED") || description == QString("PRIMARY ADDRESS") || description == QString("ADDRESS AVAILABLE") || description == QString("SlotAddressAvailable"))
			{
				if (description == QString("PRIMARY ADDRESS"))
				{
					hostEdit->setCurrentText(ip);
				}
				hostList->changeItem(QPixmap::fromMimeSource( "green.png" ),hostList->text(i),i);
				hostList->item(i)->setSelectable(true);
			}
			else if (description == QString("ADDRESS UNREACHABLE") || description == QString("SCTP_SEND_FAILED_THRESHOLD") || description == QString("SlotAddressFail"))
			{
				// renyang-modify - 加判斷避免一直改變hostList
				if (hostList->item(i)->isSelectable() || (hostList->item(i)->pixmap() == NULL))
				{
					hostList->setSelected(i,false);
					hostList->changeItem(QPixmap::fromMimeSource( "red.png" ),hostList->text(i),i);
					hostList->item(i)->setSelectable(false);
				}
			}
			else if (description == QString("SCTP_SEND_FAILED"))
			{
				// renyang-modify - 處理send fail事件
				SendFailedHandler();
			}
		}
	}
}

void CallTab::SendFailedHandler()
{
#ifdef REN_DEBUG
	qWarning("CallTab::SendFailedHandler()");
#endif
	qWarning("Hello World!!");
	if (error_handled++ > error_threshold)
	{
		// renyang-modify - 設定目前這一個primary為不能使用
		setAddressEvent(hostEdit->currentText(),QString("SCTP_SEND_FAILED_THRESHOLD"));
		qWarning("want to find a available address");
		for (unsigned int i=0;i<hostList->count();i++)
		{
			if (hostList->item(i)->isSelectable())
			{
				// renyang-modify - 設定目前這一個為primary address
				// renyang-modify - 之前send fail ip可能要設定為其它顏色的球
				// renyang-modify - 設定這一個ip為primary address
				qWarning(QString("%1 is available").arg(hostList->text(i)));
				emit setPrimaddrSignal(callId,hostList->text(i));
				error_handled = 0;
				return;
			}
		}
		// renyang-modify - 若跑到這裡的話, 則表示每一個address都是un-selectable啦, 可能要掛斷電話啦
		qWarning("There is no ip address for available");
		stopButtonClicked();
	}
}

// renyang-modify - 把由對方接收過來的影像放到video_label中
void CallTab::setVideo(QImage image)
{
#ifdef REN_DEBUG
	qWarning("CallTab::setVideo()");
#endif
	// renyang-modify - 整個封包都收到到啦, 沒有在等待剩下的封包啦
	waiting = false;
	QPixmap pix(video_label->size());
	pix.convertFromImage(image);
	video_label->setPixmap(pix);
	// renyang-modify - 當video_timer沒有在運行時, 才要主動執行videoCheckChanged()要求下一個frame
	if (!video_timer->isActive())
		videoCheckChanged();
}

// renyang-modify - 這一個也可以用來當作要取下一個frame
void CallTab::videoCheckChanged()
{
#ifdef REN_DEBUG
	qWarning(QString("CallTab::videoCheckChanged()"));
#endif
	if (video_check->isEnabled() && video_check->isChecked())
	{
		// renyang-modify - 向對方要求一個影像
		emit SigrequestPeerImage(callId);
		// renyang-modify - 設定100ms後要啟動, 到了之後, 就不在啟動了, 100ms要求一個封包
		video_timer->start(100,true);
		waiting = true;
		video_label->setBackgroundMode(Qt::NoBackground);
	}
	else
	{
		// renyang-modify - 只有當取消勾選時與對video_check做任何改變時, 會跑到這裡
		waiting = false;
		video_timer->stop();
		// renyang-modify - 因為在執行video_check->setEnabled(false);
		// renyang-modify - 也會觸動videoCheckChanged(), 所以, 在這裡要加一個判斷, 當peervideofail為true時
		// renyang-modify - 不清除video_label
		if (!peervideofail)
			clearVideoLabel();
	}
}

// renyang-modify - 向對方要求影像失敗
// renyang-modify - 當video_check是Disable, 但有被勾選則表示對方沒有辦法提供影像
void CallTab::requestImageFail()
{
#ifdef REN_DEBUG
	qWarning("CallTab::requestImageFail()");
#endif
	clearVideoLabel();
	video_label->setText(QString("Request Image Fail"));
	video_label->setAlignment( QLabel::AlignCenter );
	video_check->setEnabled(false);
	waiting = false;
	peervideofail = true;
}

void CallTab::video_timeout()
{
#ifdef REN_DEBUG
	qWarning("CallTab::video_timeout()");
#endif
	// renyang-modify - 時間時, 封包以經傳完了, 就馬上啟動要求下一個封包
	if (!waiting)
		videoCheckChanged();
}

/* 以上setVideo()與video_timer(),videoCheckChanged()的配合可以達到儘量100ms要求一個frame */

void CallTab::clearVideoLabel()
{
#ifdef REN_DEBUG
	qWarning("CallTab::clearVideoLabel()");
#endif
	video_label->clear();
	video_label->setBackgroundMode(Qt::PaletteBackground);
	video_label->setPalette(QPalette( QColor( 250, 250, 200) ));
}
