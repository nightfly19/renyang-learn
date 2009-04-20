

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

#include <qvariant.h>
#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qiconset.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>

#include <alsa/asoundlib.h>

#include "Settings.h"
#include "Error.h"

#define NAME_CUT 15

/*
 *  Constructs a Settings as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
Settings::Settings( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QTabDialog( parent, name, modal, fl ), config(Config::instance())

{
	setName( "Settings" );
	setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, sizePolicy().hasHeightForWidth() ) );
	setMinimumSize(QSize(500, 300));
	resize( QSize(500, 300).expandedTo(minimumSizeHint()) );
	
	generalWidget = new QWidget( this, "generalWidget");
	netWidget = new QWidget( this, "netWidget");
	soundWidget = new QWidget( this, "soundWidget");
	encoderWidget = new QWidget( this, "encoderWidget");
	optionsWidget = new QWidget( this, "optionsWidget");
	securityWidget = new QWidget( this, "securityWidget");
	
	QBoxLayout *gLayout = new QVBoxLayout( generalWidget, 10 );
	
	generalGroup = new QGroupBox( generalWidget, "generalGroup" );
	generalGroup->setMargin( 0 );
	generalGroup->setMidLineWidth( 0 );
	
	gLayout->addWidget(generalGroup);
	
	QBoxLayout *gtopLayout = new QVBoxLayout( generalGroup, 20 );
	
	QBoxLayout *glay1 = new QHBoxLayout( gtopLayout, 10 );
	QBoxLayout *glay2 = new QHBoxLayout( gtopLayout, 10 );
	QBoxLayout *glay3 = new QHBoxLayout( gtopLayout, 10 );
	QBoxLayout *glay4 = new QHBoxLayout( gtopLayout, 10 );
	
	nameLabel = new QLabel( generalGroup, "nameLabel" );
	
	nameEdit = new QLineEdit( generalGroup, "nameEdit" );
	nameEdit->setText( config.readEntry("/drta/general/name") );
	nameEdit->setMaxLength(DRTA_MAX_NAME_LEN);
	
	glay1->addWidget(nameLabel);
	glay1->addWidget(nameEdit);
	
	waitBox = new QCheckBox( generalGroup, "waitBox" );
	if (config.readBoolEntry("/drta/general/autowait"))
		waitBox->setChecked(TRUE);
	
	trayBox = new QCheckBox( generalGroup, "trayBox" );
	if (config.readBoolEntry("/drta/general/tray"))
		trayBox->setChecked(TRUE);
	trayBox -> setEnabled(false);
	
	glay2->addWidget(waitBox);
	glay2->addStretch();
	glay2->addWidget(trayBox);
	
	answerBox = new QCheckBox( generalGroup, "answerBox" );
	if (config.readBoolEntry("/drta/general/answer"))
		answerBox->setChecked(TRUE);
	
	hideBox = new QCheckBox( generalGroup, "hideBox" );
/*	if (config.readBoolEntry("/drta/general/autohide"))
		hideBox->setChecked(TRUE);*/
	hideBox -> setEnabled(false);
	
//	minimizeEnable(trayBox->isChecked());
	
	glay3->addWidget(answerBox);
	glay3->addStretch();
	glay3->addWidget(hideBox);
	
	dumpLabel = new QLabel( generalGroup, "dumpLabel" );
	
	openInButton = new QPushButton( generalGroup, "openInButton" );
	openInButton->setToggleButton(TRUE);
	
	openOutButton = new QPushButton( generalGroup, "openOutButton" );
	openOutButton->setToggleButton(TRUE);
	
	glay4->addWidget(dumpLabel);
	glay4->addWidget(openInButton, 2);
	glay4->addWidget(openOutButton, 2);
	
	QBoxLayout *nLayout = new QVBoxLayout( netWidget, 20 );
	
	netGroup = new QButtonGroup( netWidget, "netGroup" );
	netGroup->setMargin( 0 );
	netGroup->setMidLineWidth( 0 );
	
	nLayout->addWidget(netGroup);
	
	netOutGroup = new QButtonGroup( netWidget, "netOutGroup" );
	netOutGroup->setMargin( 0 );
	netOutGroup->setMidLineWidth( 0 );
	netOutGroup->setRadioButtonExclusive(TRUE);
	
	nLayout->addWidget(netOutGroup);
	
	QBoxLayout *ntopLayout = new QVBoxLayout( netGroup, 20 );
	QBoxLayout *nlay1 = new QHBoxLayout( ntopLayout, 10 );

	
	inprotocolLabel = new QLabel( netGroup, "inprotocolLabel" );
	
	udpBox = new QRadioButton( tr("UDP"), netGroup, "udpBox" );
	tcpBox = new QRadioButton( tr("TCP"), netGroup, "tcpBox" );
	sctpBox = new QRadioButton( tr(" SCTP TCP Style "), netGroup, "sctpBox" );
	sctp_udpBox = new QRadioButton( tr(" SCTP UDP Style "), netGroup, "sctp_udpBox" );
	sctp_udpBox -> setEnabled(false);

	if (config.readBoolEntry("/drta/net/tcp"))
		tcpBox->setChecked(TRUE);
	else if (config.readBoolEntry("/drta/net/udp"))
		udpBox->setChecked(TRUE);
	else if (config.readBoolEntry("/drta/net/sctp"))
		sctpBox->setChecked(TRUE);
	else if (config.readBoolEntry("/drta/net/sctp_udp"))
		sctp_udpBox->setChecked(TRUE);
	
	inportLabel = new QLabel( netGroup, "inportLabel" );
	
	inportBox = new QSpinBox( netGroup, "inportBox" );
	inportBox->setMaxValue( 32768 );
	inportBox->setMinValue( 1 );
	inportBox->setValue( config.readNumEntry("/drta/net/inport") );
	
	nlay1->addWidget(inprotocolLabel);
	nlay1->addWidget(udpBox);
	nlay1->addWidget(tcpBox);
	nlay1->addWidget(sctpBox);
	nlay1->addWidget(sctp_udpBox);
	nlay1->addStretch();
	nlay1->addWidget(inportLabel);
	nlay1->addWidget(inportBox);
	
	QBoxLayout *nouttopLayout = new QVBoxLayout( netOutGroup, 20);
	QBoxLayout *nlay2 = new QHBoxLayout( nouttopLayout, 10 );
	
	protocolLabel = new QLabel( netOutGroup, "protocolLabel" );
	
	udpRadioButton = new QRadioButton( tr("UDP"), netOutGroup, "udpRadioButton" );
	tcpRadioButton = new QRadioButton( tr("TCP"), netOutGroup, "tcpRadioButton" );
	sctpRadioButton = new QRadioButton( tr("SCTP TCP Style"), netOutGroup, "sctpRadioButton" );
	sctp_udpRadioButton = new QRadioButton( tr("SCTP UDP Style"), netOutGroup, "sctp_udpRadioButton" );
	sctp_udpRadioButton -> setEnabled(false);
	
	switch(config.readNumEntry("/drta/net/protocol"))
	{
		case DRTA_TCP:
		       	tcpRadioButton->setChecked(TRUE);
			break;
		case DRTA_UDP:
		       	udpRadioButton->setChecked(TRUE);
			break;
		case DRTA_SCTP:
		       	sctpRadioButton->setChecked(TRUE);
			break;
		case DRTA_SCTP_UDP:
		       	sctp_udpRadioButton->setChecked(TRUE);
			break;
	}
	
	outportLabel = new QLabel( netOutGroup, "outportLabel" );
	
	outportBox = new QSpinBox( netOutGroup, "outportBox" );
	outportBox->setMaxValue( 32768 );
	outportBox->setMinValue( 1 );
	outportBox->setValue( config.readNumEntry("/drta/net/outport") );
	
	nlay2->addWidget(protocolLabel);
	nlay2->addWidget(udpRadioButton);
	nlay2->addWidget(tcpRadioButton);
	nlay2->addWidget(sctpRadioButton);
	nlay2->addWidget(sctp_udpRadioButton);
	nlay2->addStretch();
	nlay2->addWidget(outportLabel);
	nlay2->addWidget(outportBox);
	
	QBoxLayout *sLayout = new QVBoxLayout( soundWidget, 10 );
	
	soundGroup = new QGroupBox( soundWidget, "soundGroup" );
	soundGroup->setMargin( 0 );
	soundGroup->setMidLineWidth( 0 );
	
	sLayout->addWidget(soundGroup);
	
	QBoxLayout *stopLayout = new QVBoxLayout( soundGroup, 20 );
	
	QBoxLayout *slay1 = new QHBoxLayout( stopLayout, 10 );
	QBoxLayout *slay2 = new QHBoxLayout( stopLayout, 10 );
	QBoxLayout *slay3 = new QHBoxLayout( stopLayout, 10 );
	QBoxLayout *slay4 = new QHBoxLayout( stopLayout, 10 );
	
	dspLabel = new QLabel( soundGroup, "dspInLabel" );
	
	dspBox = new QComboBox( soundGroup, "dspBox" );
	dspBox->insertItem( tr( "ALSA (input/output)" ), 0);
#ifdef HAVE_LIBJACK
	dspBox->insertItem( tr( "JACK (input) ALSA (output)" ), 1);
	dspBox->insertItem( tr( "JACK (input/output)" ), 2);
#endif
	dspBox->setCurrentItem( config.readNumEntry("/drta/sound/inputdriver") + config.readNumEntry("/drta/sound/outputdriver"));
	
	interfInLabel = new QLabel( soundGroup, "interfInLabel" );
	
	interfInBox = new QComboBox( TRUE, soundGroup, "interfaceBox" );
	interfInBox->setAutoCompletion(TRUE);
	interfInBox->setDuplicatesEnabled(FALSE);
	interfInBox->clear();
	
	interfOutLabel = new QLabel( soundGroup, "interfOutLabel" );
	
	interfOutBox = new QComboBox( TRUE, soundGroup, "interfaceBox" );
	interfOutBox->setAutoCompletion(TRUE);
	interfOutBox->setDuplicatesEnabled(FALSE);
	interfOutBox->clear();
	
	int card = -1;
	if (snd_card_next(&card) < 0 || card < 0)
		QMessageBox::warning(0, "Drta Warning", "Warning: no soundcards found!");
	
	interfInBox->insertItem( tr( "default" ) );
	interfOutBox->insertItem( tr( "default" ) );
	
	while (card >= 0)
	{
		QString interface = QString("plughw:%1").arg(card);
		interfInBox->insertItem( interface );
		interfOutBox->insertItem( interface );
		
		interface = QString("hw:%1").arg(card);
		interfInBox->insertItem( interface );
		interfOutBox->insertItem( interface );
		
		if (snd_card_next(&card) < 0)
			break;
	}
	
	interfInBox->setEditText(config.readEntry("/drta/sound/inputinterface"));
	interfOutBox->setEditText(config.readEntry("/drta/sound/outputinterface"));
	
	slay1->addWidget(dspLabel);
	slay1->addWidget(dspBox, 1);
	
	modeLabel = new QLabel( soundGroup, "modeLabel" );
	
	modeBox = new QComboBox( soundGroup, "modeBox" );
	modeBox->insertItem( tr( "narrow (8000 Hz, 16 bit, Mono)" ), DRTA_NARROW);
	modeBox->insertItem( tr( "wide (16000 Hz, 16 bit, Mono)" ), DRTA_WIDE);
	modeBox->insertItem( tr( "ultra-wide (32000 Hz, 16 bit, Mono)" ), DRTA_ULTRAWIDE);
	modeBox->setCurrentItem( config.readNumEntry("/drta/speex/mode") );
	
	slay2->addWidget(interfInLabel);
	slay2->addWidget(interfInBox);
	slay2->addStretch();
	slay2->addWidget(interfOutLabel);
	slay2->addWidget(interfOutBox);
	
	slay3->addWidget(modeLabel);
	slay3->addWidget(modeBox, 1);
	
	stoptxLabel = new QLabel( soundGroup, "stoptxLabel" );
	
	stoptxBox = new QSpinBox( soundGroup, "stoptxBox" );
	stoptxBox->setMaxValue( 30 );
	stoptxBox->setMinValue( 0 );
	stoptxBox->setValue( config.readNumEntry("/drta/sound/txstop") );
	
	ringVolumeLabel = new QLabel( soundGroup, "ringVolumeLabel" );
	
	ringVolumeBox = new QComboBox( soundGroup, "ringVolumeBox" );
	ringVolumeBox->insertItem( tr( "Off" ), 0);
	ringVolumeBox->insertItem( tr( "High" ), 1);
	ringVolumeBox->insertItem( tr( "Medium" ), 2);
	ringVolumeBox->insertItem( tr( "Low" ), 3);
	ringVolumeBox->setCurrentItem( config.readNumEntry("/drta/sound/ringvolume") );
	
	slay4->addStretch(1);
	slay4->addWidget(stoptxLabel);
	slay4->addWidget(stoptxBox);
	slay4->addStretch(1);
	slay4->addWidget(ringVolumeLabel);
	slay4->addWidget(ringVolumeBox);
	slay4->addStretch(1);
	
	QBoxLayout *eLayout = new QVBoxLayout( encoderWidget, 10 );
	
	encoderGroup = new QGroupBox( encoderWidget, "soundGroup" );
	encoderGroup->setMargin( 0 );
	encoderGroup->setMidLineWidth( 0 );
	
	eLayout->addWidget(encoderGroup);
	
	QBoxLayout *etopLayout = new QVBoxLayout( encoderGroup, 20);
	
	QBoxLayout *elay1 = new QHBoxLayout( etopLayout, 10 );
	QBoxLayout *elay2 = new QHBoxLayout( etopLayout, 10 );
	QBoxLayout *elay3 = new QHBoxLayout( etopLayout, 10 );
	
	bitrateLabel = new QLabel( encoderGroup, "bitrateLabel" );
	
	bitrateBox = new QComboBox( encoderGroup, "bitrateBox" );
	bitrateBox->insertItem( tr( "CBR" ), 0);
	bitrateBox->insertItem( tr( "VBR" ), 1);
	bitrateBox->insertItem( tr( "ABR" ), 2);
	bitrateBox->setCurrentItem( config.readNumEntry("/drta/speex/bitratemode") );
	
	complexityLabel = new QLabel( encoderGroup, "complexityLabel" );
	
	complexityBox = new QSpinBox( encoderGroup, "complexityBox" );
	complexityBox->setMaxValue( 10 );
	complexityBox->setMinValue( 1 );
	complexityBox->setValue( config.readNumEntry("/drta/speex/complexity") );
	
	elay1->addWidget(bitrateLabel);
	elay1->addWidget(bitrateBox, 1);
	elay1->addWidget(complexityLabel);
	elay1->addWidget(complexityBox);
	
	cbrqualityLabel = new QLabel( encoderGroup, "cbrqualityLabel" );
	
	cbrqualityBox = new QSpinBox( encoderGroup, "cbrqualityBox" );
	cbrqualityBox->setMaxValue( 10 );
	cbrqualityBox->setMinValue( 0 );
	cbrqualityBox->setValue( config.readNumEntry("/drta/speex/quality") );
	
	vbrqualityLabel = new QLabel( encoderGroup, "vbrqualityLabel" );
	
	vbrqualityBox = new QSpinBox( encoderGroup, "vbrqualityBox" );
	vbrqualityBox->setMaxValue( 10 );
	vbrqualityBox->setMinValue( 0 );
	vbrqualityBox->setValue( config.readNumEntry("/drta/speex/vbrquality") );
	
	abrLabel = new QLabel( encoderGroup, "abrLabel" );
	
	abrBox = new QSpinBox( encoderGroup, "abrBox" );
	abrBox->setMaxValue( 44 );
	abrBox->setMinValue( 2 );
	abrBox->setValue( config.readNumEntry("/drta/speex/abr") );
	
	elay2->addWidget(cbrqualityLabel);
	elay2->addWidget(cbrqualityBox);
	elay2->addWidget(vbrqualityLabel);
	elay2->addWidget(vbrqualityBox);
	elay2->addWidget(abrLabel);
	elay2->addWidget(abrBox);
	
	vadBox = new QCheckBox( encoderGroup, "vadBox" );
	if (config.readBoolEntry("/drta/speex/vad"))
		vadBox->setChecked(TRUE);
	
	dtxBox = new QCheckBox( encoderGroup, "dtxBox" );
	if (config.readBoolEntry("/drta/speex/dtx"))
		dtxBox->setChecked(TRUE);
	
	elay3->addWidget(vadBox);
	elay3->addStretch();
	elay3->addWidget(dtxBox);
	
	QBoxLayout *oLayout = new QVBoxLayout( optionsWidget, 10 );
	
	adrGroup = new QGroupBox( optionsWidget, "adrGroup" );
	adrGroup->setMargin( 0 );
	adrGroup->setMidLineWidth( 0 );
	
	oLayout->addWidget(adrGroup);
	
	agcGroup = new QButtonGroup( optionsWidget, "agcGroup" );
	agcGroup->setMargin( 0 );
	agcGroup->setMidLineWidth( 0 );
	
	oLayout->addWidget(agcGroup);
	
	QBoxLayout *otopLayout = new QVBoxLayout( adrGroup, 20 );
	otopLayout->addSpacing(5);
	QBoxLayout *olay1 = new QHBoxLayout( otopLayout, 10 );
	QBoxLayout *olay2 = new QHBoxLayout( otopLayout, 10 );
	
	adrmindelayLabel = new QLabel( adrGroup, "adrmindelayLabel" );
	
	adrmindelayBox = new QSpinBox( adrGroup, "adrmindelayBox" );
	adrmindelayBox->setMaxValue( 9999 );
	adrmindelayBox->setMinValue( 0 );
	adrmindelayBox->setValue( config.readNumEntry("/drta/sound/adrmindelay") );
	
	adrmaxdelayLabel = new QLabel( adrGroup, "adrmaxdelayLabel" );
	
	adrmaxdelayBox = new QSpinBox( adrGroup, "adrmaxdelayBox" );
	adrmaxdelayBox->setMaxValue( 9999 );
	adrmaxdelayBox->setMinValue( 1 );
	adrmaxdelayBox->setValue( config.readNumEntry("/drta/sound/adrmaxdelay") );
	
	adrstretchLabel = new QLabel( adrGroup, "adrstretchLabel" );
	
	adrstretchBox = new QSpinBox( adrGroup, "adrstretchBox" );
	adrstretchBox->setMaxValue( 50 );
	adrstretchBox->setMinValue( 1 );
	adrstretchBox->setValue( config.readNumEntry("/drta/sound/adrstretch") );
	
	olay1->addStretch(1);
	olay1->addWidget(adrstretchLabel, 2);
	olay1->addWidget(adrstretchBox, 2);
	olay1->addStretch(1);
	
	olay2->addWidget(adrmindelayLabel);
	olay2->addWidget(adrmindelayBox);
	olay2->addStretch();
	olay2->addWidget(adrmaxdelayLabel);
	olay2->addWidget(adrmaxdelayBox);
	
	QBoxLayout *ootopLayout = new QVBoxLayout( agcGroup, 20);
	ootopLayout->addSpacing(5);
	QBoxLayout *olay3 = new QHBoxLayout( ootopLayout, 10 );
	QBoxLayout *olay4 = new QHBoxLayout( ootopLayout, 10 );
	
	agcSpeedLabel = new QLabel( agcGroup, "agcSpeedLabel" );
	
	agcstepBox = new QSpinBox( agcGroup, "agcstepBox" );
	agcstepBox->setMaxValue( 1000 );
	agcstepBox->setMinValue( 1 );
	agcstepBox->setValue( config.readNumEntry("/drta/sound/agcstep") );
	
	agcminLabel = new QLabel( agcGroup, "agcminLabel" );
	
	agcminBox = new QSpinBox( agcGroup, "agcminBox" );
	agcminBox->setMaxValue( 50 );
	agcminBox->setMinValue( 0 );
	agcminBox->setValue( config.readNumEntry("/drta/sound/agcmin") );
	
	agcmaxLabel = new QLabel( agcGroup, "agcmaxLabel" );
	
	agcmaxBox = new QSpinBox( agcGroup, "agcstepBox" );
	agcmaxBox->setMaxValue( 50 );
	agcmaxBox->setMinValue( 1 );
	agcmaxBox->setValue( config.readNumEntry("/drta/sound/agcmax") );
	
	olay3->addStretch(1);
	olay3->addWidget(agcSpeedLabel, 2);
	olay3->addWidget(agcstepBox, 2);
	olay3->addStretch(1);
	
	olay4->addWidget(agcminLabel);
	olay4->addWidget(agcminBox);
	olay4->addStretch();
	olay4->addWidget(agcmaxLabel);
	olay4->addWidget(agcmaxBox);
	
	QBoxLayout *kLayout = new QVBoxLayout( securityWidget, 10 );
	
	securityGroup = new QButtonGroup( securityWidget, "securityGroup" );
	securityGroup->setRadioButtonExclusive(TRUE);
	securityGroup->setMargin( 0 );
	securityGroup->setMidLineWidth( 0 );
	
	kLayout->addWidget(securityGroup);
	
	QBoxLayout *ktopLayout = new QVBoxLayout( securityGroup, 20 );
	
	QBoxLayout *klay1 = new QHBoxLayout( ktopLayout, 10 );
	QBoxLayout *klay2 = new QHBoxLayout( ktopLayout, 10 );
	QBoxLayout *klay3 = new QHBoxLayout( ktopLayout, 10 );
	
	keyLabel = new QLabel( securityGroup, "keyLabel" );
	
	randomRadioButton = new QRadioButton( tr(""), securityGroup, "randomRadioButton" );
	
	bitsBox = new QSpinBox( securityGroup, "bitsBox" );
	bitsBox->setLineStep( 8 );
	bitsBox->setMaxValue( 448 );
	bitsBox->setMinValue( 8 );
	bitsBox->setValue( config.readNumEntry("/drta/security/keylen") );
	
	passwordRadioButton = new QRadioButton( tr(""), securityGroup, "passwordRadioButton" );
	
	if (config.readBoolEntry("/drta/security/random"))
		randomRadioButton->setChecked(TRUE);
	else
		passwordRadioButton->setChecked(TRUE);
	
	QBoxLayout *hklay1 = new QHBoxLayout( klay1 );
	QBoxLayout *kklay1 = new QHBoxLayout( hklay1 );
		kklay1->addWidget(keyLabel);
	QBoxLayout *hklay2 = new QHBoxLayout( klay1 );
	QBoxLayout *kklay2 = new QVBoxLayout( hklay2 );
	QBoxLayout *kkklay1 = new QHBoxLayout( kklay2 );
		kkklay1->addWidget(randomRadioButton);
		kkklay1->addWidget(bitsBox);
	kklay2->addSpacing(10);
	QBoxLayout *kkklay2 = new QHBoxLayout( kklay2 );
		kkklay2->addWidget(passwordRadioButton);
	
	showkeyBox = new QCheckBox( securityGroup, "showkeyBox" );
	if (config.readBoolEntry("/drta/security/showkey"))
		showkeyBox->setChecked(TRUE);
	
	klay2->addWidget(showkeyBox);
	
	logLabel = new QLabel( securityGroup, "logLabel" );
	
	logButton = new QPushButton( securityGroup, "logButton" );
	logButton->setToggleButton(TRUE);
	
	klay3->addWidget(logLabel);
	klay3->addWidget(logButton);
	
	inFile = config.readEntry("/drta/general/dumpin");
	if (!inFile.isEmpty())
		openInButton->toggle();
	
	outFile = config.readEntry("/drta/general/dumpout");
	if (!outFile.isEmpty())
		openOutButton->toggle();
	
	logFile = config.readEntry("/drta/security/logfile");
	if (!logFile.isEmpty())
		logButton->toggle();
	
	languageChange();
	bitrateChanged();
	
	addTab(generalWidget, tr("&General"));
	addTab(netWidget, tr("&Network"));
	addTab(soundWidget, tr("&Sound"));
	addTab(encoderWidget, "&Encoder");
	addTab(optionsWidget, "&Options");
	addTab(securityWidget, tr("Se&curity"));
	
	connect( this, SIGNAL( applyButtonPressed() ), this, SLOT( saveSettings() ) );
	connect( this, SIGNAL( defaultButtonPressed() ), this, SLOT( defaults() ) );
	connect( openInButton, SIGNAL( toggled(bool) ), this, SLOT( dumpinEnable(bool) ) );
	connect( openOutButton, SIGNAL( toggled(bool) ), this, SLOT( dumpoutEnable(bool) ) );
	connect( bitrateBox, SIGNAL( activated(int) ), this, SLOT( bitrateChanged() ) );
	connect( logButton, SIGNAL( toggled(bool) ), this, SLOT( logEnable(bool) ) );
	connect( trayBox, SIGNAL( toggled(bool) ), this, SLOT( minimizeEnable(bool) ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
Settings::~Settings()
{
	// no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Settings::languageChange()
{
	setCaption( tr( "Settings" ) );
	generalGroup->setTitle( tr( "General settings" ) );
	netGroup->setTitle( tr( "Receiver settings (for incoming calls)" ) );
	netOutGroup->setTitle( tr( "Caller settings (for outgoing calls)" ) );
//	netInGroup->setTitle( tr( "Server settings (for incoming calls)" ) );
	soundGroup->setTitle( tr( "Sound settings" ) );
	encoderGroup->setTitle( tr( "Speex settings" ) );
	adrGroup->setTitle( tr( "Audio Delay Reduction (ADR)" ) );
	agcGroup->setTitle( tr( "Automatic Gain Control (AGC)" ) );
	securityGroup->setTitle( tr( "Security and cryptography settings" ) );
	
	nameLabel->setText( tr( "My name" ) );
	answerBox->setText( tr( "Answer automatically the calls" ) );
	trayBox->setText( tr( "Show tray icon" ) );
	waitBox->setText( tr( "Wait for incoming calls at startup" ) );
	hideBox->setText( tr( "Start minimized" ) );
	dumpLabel->setText( tr( "Record stream to file" ) );
	if (openInButton->isOn())
		openInButton->setText("RX: " + fileName(inFile));
	else
		dumpinEnable(false);
	if (openOutButton->isOn())
		openOutButton->setText("TX: " + fileName(outFile));
	else
		dumpoutEnable(false);
	inprotocolLabel->setText( tr( "Allowed protocols:" ) );
	protocolLabel->setText( tr( "Preferred protocol:" ) );
	udpBox->setText( tr( "UDP" ) );
	tcpBox->setText( tr( "TCP" ) );
	sctpBox->setText( tr( " SCTP TCP Style " ) );
	sctp_udpBox->setText( tr( " SCTP UDP Style " ) );
	inportLabel->setText( tr( "Port:" ) );
	outportLabel->setText( tr( "Port:" ) );
	dspLabel->setText( tr( "Sound driver" ) );
	interfInLabel->setText( tr( "Input interface" ) );
	interfOutLabel->setText( tr( "Output interface" ) );
	stoptxLabel->setText( tr( "Stop TX after (silence secs)" ) );
	ringVolumeLabel->setText( tr( "Ring Volume" ) );
	adrmindelayLabel->setText( tr( "Minimum delay (ms)" ) );
	adrmaxdelayLabel->setText( tr( "Maximum delay (ms)" ) );
	adrstretchLabel->setText( tr( "Time change percentage (0-50 \%)") );
	agcSpeedLabel->setText( tr( "Volume change speed (1-100)" ) );
	agcminLabel->setText( tr( "Minimum gain factor" ) );
	agcmaxLabel->setText( tr( "Maximum gain factor" ) );
	modeLabel->setText( tr( "Input format" ) );
	bitrateLabel->setText( tr( "Bitrate mode" ) );
	complexityLabel->setText( tr( "Complexity" ) );
	cbrqualityLabel->setText( tr( "CBR Quality" ) );
	abrLabel->setText( tr( "ABR kbps" ) );
	vbrqualityLabel->setText( tr( "VBR Quality" ) );
	vadBox->setText( tr( "Voice Activity Detection (VAD)" ) );
	dtxBox->setText( tr( "Discontinuous TX" ) );
	showkeyBox->setText( tr( "Show decryption key of incoming stream in the Log" ) );
	passwordRadioButton->setText( tr( "Text passphrase (max 56 chars)" ) );
	randomRadioButton->setText( tr( "Random with constant length (bits):" ) );
	keyLabel->setText( tr( "Encryption key type:" ) );
	logLabel->setText( tr( "Write a copy of the Log also to file"));
	if (logButton->isOn())
		logButton->setText(fileName(logFile));
	else
		logEnable(false);
	setOkButton(QString::null);
	setApplyButton("OK");
	setCancelButton();
	setDefaultButton();
	
	QWhatsThis::add(nameEdit, tr("Your name, nickname, address, or whatever you want that your partner will see in order to recognize you. This information will be shown only to the person who's talking with you (leave empty to be anonymous)."));
	QWhatsThis::add(answerBox, tr("When you check this option, DRTA will automatically answer the incoming calls, without need to click on Answer button."));
	QWhatsThis::add(trayBox, tr("Enable/disable the DRTA icon in the system tray."));
	QWhatsThis::add(waitBox, tr("To wait for calls immediately when you launch DRTA, without need to click on Wait for calls button."));
	QWhatsThis::add(hideBox, tr("This will hide the main window and will automatically minimize DRTA to system tray at startup."));
	QWhatsThis::add(openInButton, tr("Enable/disable recording of incoming stream to file. Click to select a file, or to disable."));
	QWhatsThis::add(openOutButton, tr("Enable/disable recording of outgoing stream to file. Click to select a file, or to disable."));
	QWhatsThis::add(udpBox, tr("Enable/disable receiving calls with UDP protocol."));
	QWhatsThis::add(tcpBox, tr("Enable/disable receiving calls with TCP protocol."));
	QWhatsThis::add(inportBox, tr("This port will be used to receive calls on the selected protocols."));
	QWhatsThis::add(udpRadioButton, tr("UDP is the default protocol. Choose this protocol for standard calls."));
	QWhatsThis::add(tcpRadioButton, tr("Choose TCP if remote Receiver only supports this protocol, or you prefer reliable connections."));
	QWhatsThis::add(outportBox, tr("This port is the Receiver port of the remote computer that you want to call. It will be used for your outgoing calls."));
	QWhatsThis::add(dspBox, tr("Sound driver for capture and playback. ALSA is the default driver."));
	QWhatsThis::add(interfInBox, tr("Input sound interface (ALSA). I recommend using plughw or hw interface, but if you need to use more capture streams, then choose another interface. If you have more than one soundcard, you can choose the sound card by changing the device number (for example hw:2)."));
	QWhatsThis::add(interfOutBox, tr("Output sound interface (ALSA). I recommend using plughw or hw interface. If you have more than one soundcard, you can choose the sound card by changing the device number (for example hw:2)."));
	QWhatsThis::add(modeBox, tr("The input format determines the audio quality and the network usage for your voice. To improve the sound quality of your voice, choose a higher sample rate."));
	QWhatsThis::add(stoptxBox, tr("When you are using a threshold greater than zero, it's useful to continue the transmission for some seconds, to preserve the stream against small speech pauses."));
	QWhatsThis::add(ringVolumeBox, tr("Here you can adjust the volume of ring tone for incoming calls."));
	QWhatsThis::add(adrmindelayBox, tr("This is the minimum audio delay on the output sound interface. When ADR is activated, if the delay is smaller than maximum, DRTA will try to increase the delay to this value in order to avoid sound crackle."));
	QWhatsThis::add(adrmaxdelayBox, tr("This is the maximum audio delay on the output sound interface. When ADR is activated, if the delay is greater than minimum, DRTA will try to reduce the delay to this value."));
	QWhatsThis::add(adrstretchBox, tr("This is the time change percentage for ADR. If the current playback delay is smaller/greater than reference values, than DRTA will adjust the current audio stream. A higher time stretch means faster adjustment."));
	QWhatsThis::add(agcstepBox, tr("This value will influence the rapidity of AGC volume adjustment (you can change the reference volume from the main window)."));
	QWhatsThis::add(agcminBox, tr("The minimum amplitude value. If you think that the volume is getting too low, you can adjust this value."));
	QWhatsThis::add(agcmaxBox, tr("The maximum amplitude value. If you think that the volume is getting too high, you can adjust this value to limit the amplitude. This is useful to avoid excessive silence amplification."));
	QWhatsThis::add(bitrateBox, tr("Choose the encoding bitrate type for your voice beetween: Constant (CBR), Variable (VBR), Average (ABR)."));
	QWhatsThis::add(complexityBox, tr("Algorithmic quality. 0 means less CPU computation but lower quality, 10 means highest quality but more computation."));
	QWhatsThis::add(cbrqualityBox, tr("CBR quality: 0 lowest bitrate, 10 highest bitrate."));
	QWhatsThis::add(vbrqualityBox, tr("VBR quality: 0 lowest bitrate, 10 highest bitrate."));
	QWhatsThis::add(abrBox, tr("ABR bitrate (kilobit per second). You can specify your preferred bitrate average value."));
	QWhatsThis::add(vadBox, tr("When enabled, it detects whether the audio being encoded is speech or silence/background noise. VAD is implicitly activated when encoding is VBR."));
	QWhatsThis::add(dtxBox, tr("It's an addition to VAD/VBR operation, that allows to stop transmitting completely when background noise is stationary."));
	QWhatsThis::add(randomRadioButton, tr("DRTA will automatically choose a new random key."));
	QWhatsThis::add(passwordRadioButton, tr("DRTA will ask you a passphrase when you enable encryption or when you change the encryption key."));
	QWhatsThis::add(bitsBox, tr("Key length (in bits) of random key."));
	QWhatsThis::add(showkeyBox, tr("This option will show the decryption passphrase of the incoming stream in the Log. Be careful, this might show confidential informations!"));
	QWhatsThis::add(logButton, tr("To save the Log (the one you open with View Log button) to file."));
}

void Settings::saveSettings()
{
	try
	{
		if (dtxBox->isChecked() && !(vadBox->isChecked() || (bitrateBox->currentItem())))
			QMessageBox::warning(0, "Drta Warning", "Warning: Discontinuos TX is useless without VAD, VBR or ABR.");
		
		if (inportBox->value() < 1025)
			QMessageBox::warning(0, "Drta Warning", "Warning: you must be root to use a port lower than 1025.");
		
		if (dspBox->currentItem() > 0)
			QMessageBox::warning(0, "Drta Warning", "Warning: you chose to use Jack as input sound driver.\nPlease ensure that Jack is running with correct settings.");
		
		if (!udpBox->isChecked() && !tcpBox->isChecked() &&!sctpBox->isChecked() && !sctp_udpBox->isChecked() )
			throw Error("no Receiver protocol was selected.\nPlease choose at least one protocol.");
		
		if (agcmaxBox->value() < agcminBox->value())
			throw Error("AGC Max gain is smaller than Min gain!");
		
		if (adrmaxdelayBox->value() < adrmindelayBox->value())
			throw Error("ADR Max delay is smaller than Min delay!");
		
		config.writeEntry("/drta/general/name", nameEdit->text());
		config.writeEntry("/drta/general/answer", answerBox->isChecked());
//		config.writeEntry("/drta/general/tray", trayBox->isChecked());
		config.writeEntry("/drta/general/autowait", waitBox->isChecked());
//		config.writeEntry("/drta/general/autohide", hideBox->isChecked());
		config.writeEntry("/drta/general/dumpin", inFile);
		config.writeEntry("/drta/general/dumpout", outFile);
	
		config.writeEntry("/drta/net/udp", udpBox->isChecked());
		config.writeEntry("/drta/net/tcp", tcpBox->isChecked());
		config.writeEntry("/drta/net/sctp", sctpBox->isChecked());
		config.writeEntry("/drta/net/sctp_udp", sctp_udpBox->isChecked());
		config.writeEntry("/drta/net/inport", inportBox->value());
		if (tcpRadioButton->isChecked())
			config.writeEntry("/drta/net/protocol", DRTA_TCP);
		else if (udpRadioButton->isChecked())
			config.writeEntry("/drta/net/protocol", DRTA_UDP);
		else if (sctpRadioButton -> isChecked() ) 
			config.writeEntry("/drta/net/protocol", DRTA_SCTP);
		else
			config.writeEntry("/drta/net/protocol", DRTA_SCTP_UDP);
		config.writeEntry("/drta/net/outport", outportBox->value());
		
		switch(dspBox->currentItem())
		{
			case 0:
				config.writeEntry("/drta/sound/inputdriver", 0);
				config.writeEntry("/drta/sound/outputdriver", 0);
				break;
			case 1:
				config.writeEntry("/drta/sound/inputdriver", 1);
				config.writeEntry("/drta/sound/outputdriver", 0);
				break;
			case 2:
				config.writeEntry("/drta/sound/inputdriver", 1);
				config.writeEntry("/drta/sound/outputdriver", 1);
				break;
		}
		config.writeEntry("/drta/sound/inputinterface", interfInBox->currentText());
		config.writeEntry("/drta/sound/outputinterface", interfOutBox->currentText());
		config.writeEntry("/drta/sound/txstop", stoptxBox->value());
		config.writeEntry("/drta/sound/ringvolume", ringVolumeBox->currentItem());
		config.writeEntry("/drta/sound/agcstep", agcstepBox->value());
		config.writeEntry("/drta/sound/agcmin", agcminBox->value());
		config.writeEntry("/drta/sound/agcmax", agcmaxBox->value());
		config.writeEntry("/drta/sound/adrmindelay", adrmindelayBox->value());
		config.writeEntry("/drta/sound/adrmaxdelay", adrmaxdelayBox->value());
		config.writeEntry("/drta/sound/adrstretch", adrstretchBox->value());
		
		config.writeEntry("/drta/speex/complexity", complexityBox->value());
		config.writeEntry("/drta/speex/quality", cbrqualityBox->value());
		config.writeEntry("/drta/speex/vbrquality", vbrqualityBox->value());
		config.writeEntry("/drta/speex/mode", modeBox->currentItem());
		config.writeEntry("/drta/speex/bitratemode", bitrateBox->currentItem());
		config.writeEntry("/drta/speex/abr", abrBox->value());
		config.writeEntry("/drta/speex/vad", vadBox->isChecked());
		config.writeEntry("/drta/speex/dtx", dtxBox->isChecked());
		
		config.writeEntry("/drta/security/logfile", logFile);
		config.writeEntry("/drta/security/showkey", showkeyBox->isChecked());
		config.writeEntry("/drta/security/random", randomRadioButton->isChecked());
		config.writeEntry("/drta/security/keylen", bitsBox->value());
		
		accept();
	}
	catch (Error e)
	{
		QMessageBox::critical(0, "Drta Settings Error", "Error: " + e.getText());
	}
}

void Settings::defaults()
{
	inportBox->setValue(DRTA_DEFAULT_INPORT);
	outportBox->setValue(DRTA_DEFAULT_OUTPORT);
	udpBox->setChecked(DRTA_DEFAULT_UDP);
	tcpBox->setChecked(DRTA_DEFAULT_TCP);
	sctpBox->setChecked(DRTA_DEFAULT_SCTP);
	sctp_udpBox->setChecked(DRTA_DEFAULT_SCTP_UDP);
	answerBox->setChecked(DRTA_DEFAULT_ANSWER);
	trayBox->setChecked(DRTA_DEFAULT_TRAY);
	waitBox->setChecked(DRTA_DEFAULT_WAIT);
	hideBox->setChecked(DRTA_DEFAULT_HIDE);
	modeBox->setCurrentItem(DRTA_DEFAULT_MODE);
	bitrateBox->setCurrentItem(DRTA_DEFAULT_BITRATEMODE);
	stoptxBox->setValue(DRTA_DEFAULT_TXSTOP);
	ringVolumeBox->setCurrentItem(DRTA_DEFAULT_RINGVOLUME);
	complexityBox->setValue(DRTA_DEFAULT_COMPLEXITY);
	cbrqualityBox->setValue(DRTA_DEFAULT_QUALITY);
	vbrqualityBox->setValue(DRTA_DEFAULT_VBRQUALITY);
	abrBox->setValue(DRTA_DEFAULT_ABR);
	vadBox->setChecked(DRTA_DEFAULT_VAD);
	dtxBox->setChecked(DRTA_DEFAULT_DTX);
	dspBox->setCurrentItem(DRTA_DEFAULT_INDRIVER + DRTA_DEFAULT_OUTDRIVER);
	interfInBox->setCurrentText(DRTA_DEFAULT_INTERFACE);
	interfOutBox->setCurrentText(DRTA_DEFAULT_INTERFACE);
	agcstepBox->setValue(DRTA_DEFAULT_AGCSTEP);
	agcminBox->setValue(DRTA_DEFAULT_AGCMIN);
	agcmaxBox->setValue(DRTA_DEFAULT_AGCMAX);
	adrmindelayBox->setValue(DRTA_DEFAULT_ADRMINDELAY);
	adrmaxdelayBox->setValue(DRTA_DEFAULT_ADRMAXDELAY);
	adrstretchBox->setValue(DRTA_DEFAULT_ADRSTRETCH);
	bitsBox->setValue(DRTA_DEFAULT_KEYLEN);
	udpRadioButton->setChecked(TRUE);
	randomRadioButton->setChecked(DRTA_DEFAULT_RANDOM);
	showkeyBox->setChecked(DRTA_DEFAULT_SHOWKEY);
	bitrateChanged();
}

void Settings::logEnable(bool on)
{
	if (on)
	{
		QString name=QFileDialog::getSaveFileName(config.readEntry("/drta/security/logfile"),"*.*",this,0,"Save to file...");
		if (!name.isEmpty())
		{
			logFile = name;
			logButton->setText(fileName(logFile));
			logButton->setIconSet( QIconSet( QPixmap() ) );
		}
		else
			logButton->toggle();
	}
	else
	{
		logButton->setIconSet( QIconSet( QPixmap::fromMimeSource( "open.png" ) ) );
		logButton->setText(tr("Disabled"));
		logFile = "";
	}
}

void Settings::dumpinEnable(bool on)
{
	if (on)
	{
		QString name=QFileDialog::getSaveFileName(config.readEntry("/drta/general/dumpin"),"*"DRTA_EXT,this,0,"Save to file...");
		if (!name.isEmpty())
		{
			if (!name.endsWith(DRTA_EXT))
				name += DRTA_EXT;
			inFile = name;
			openInButton->setIconSet( QIconSet( QPixmap() ) );
			openInButton->setText(QString("RX: %1").arg(fileName(inFile)));
		}
		else
			openInButton->toggle();
	}
	else
	{
		openInButton->setIconSet( QIconSet( QPixmap::fromMimeSource( "rec.png" ) ) );
		openInButton->setText(tr("RX: disabled"));
		inFile = "";
	}
}

void Settings::dumpoutEnable(bool on)
{
	if (on)
	{
		QString name=QFileDialog::getSaveFileName(config.readEntry("/drta/general/dumpout"),"*"DRTA_EXT,this,0,"Save to file...");
		if (!name.isEmpty())
		{
			if (!name.endsWith(DRTA_EXT))
				name += DRTA_EXT;
			outFile = name;
			openOutButton->setIconSet( QIconSet( QPixmap() ) );
			openOutButton->setText(QString("TX: %1").arg(fileName(outFile)));
		}
		else
			openOutButton->toggle();
	}
	else
	{
		openOutButton->setIconSet( QIconSet( QPixmap::fromMimeSource( "rec.png" ) ) );
		openOutButton->setText(tr("TX: disabled"));
		outFile = "";
	}
}

void Settings::bitrateChanged()
{
    switch(bitrateBox->currentItem())
    {
	case DRTA_NARROW:
		cbrqualityBox->setEnabled(TRUE);
		vbrqualityBox->setEnabled(FALSE);
		abrBox->setEnabled(FALSE);
		break;
	case DRTA_WIDE:
		cbrqualityBox->setEnabled(FALSE);
		vbrqualityBox->setEnabled(TRUE);
		abrBox->setEnabled(FALSE);
		break;
	case DRTA_ULTRAWIDE:
		cbrqualityBox->setEnabled(FALSE);
		vbrqualityBox->setEnabled(FALSE);
		abrBox->setEnabled(TRUE);
		break;
    }
}

void Settings::minimizeEnable(bool on)
{
	hideBox->setEnabled(on);
	if (!on)
		hideBox->setChecked(FALSE);
}

QString Settings::fileName(QString name)
{
	if (name.length() < NAME_CUT)
		return name;
	else
		return QString("....%1").arg(name.right(NAME_CUT));
}
