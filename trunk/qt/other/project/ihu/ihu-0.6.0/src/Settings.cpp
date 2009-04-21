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
#include <qslider.h>
#include <qinputdialog.h>

#include <alsa/asoundlib.h>

#include "Settings.hpp"
#include "Error.h"

#define NAME_CUT 30

/*
 *  Constructs a Settings as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
Settings::Settings( Config& ihucfg, QWidget* parent, const char* name, bool modal, WFlags fl )
    : QTabDialog( parent, name, modal, fl ), ihuconfig(ihucfg)

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
	nameEdit->setText( ihuconfig.getMyName() );
	nameEdit->setMaxLength(IHU_MAX_NAME_LEN);
	
	glay1->addWidget(nameLabel);
	glay1->addWidget(nameEdit);
	
	waitBox = new QCheckBox( generalGroup, "waitBox" );
	if (ihuconfig.getAutoWait())
		waitBox->setChecked(TRUE);
	
	trayBox = new QCheckBox( generalGroup, "trayBox" );
	if (ihuconfig.getTrayIcon())
		trayBox->setChecked(TRUE);
	
	glay2->addWidget(waitBox);
	glay2->addStretch();
	glay2->addWidget(trayBox);
	
	answerBox = new QCheckBox( generalGroup, "answerBox" );
	if (ihuconfig.getAutoAnswer())
		answerBox->setChecked(TRUE);
	
	hideBox = new QCheckBox( generalGroup, "hideBox" );
	if (ihuconfig.getAutoHide())
		hideBox->setChecked(TRUE);
	
	minimizeEnable(trayBox->isChecked());
	
	glay3->addWidget(answerBox);
	glay3->addStretch();
	glay3->addWidget(hideBox);
	
	maxCallLabel = new QLabel( generalGroup, "maxCallLabel" );
	
	maxCallBox = new QSpinBox( generalGroup, "maxCallBox" );
	maxCallBox->setRange( 1, IHU_MAX_CALLS );
	maxCallBox->setValue( ihuconfig.getMaxCalls() );

	maxHostLabel = new QLabel( generalGroup, "maxHostLabel" );
	
	maxHostBox = new QSpinBox( generalGroup, "maxHostBox" );
	maxHostBox->setRange( 0, IHU_MAX_HOSTS );
	maxHostBox->setValue( ihuconfig.getMaxHosts() );

	glay4->addWidget(maxCallLabel);
	glay4->addWidget(maxCallBox);
	glay4->addStretch();
	glay4->addWidget(maxHostLabel);
	glay4->addWidget(maxHostBox);

	QBoxLayout *nLayout = new QVBoxLayout( netWidget, 20 );
	
	netGroup = new QGroupBox( netWidget, "netGroup" );
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
	
	udpBox = new QCheckBox( netGroup, "udpBox" );
	if (ihuconfig.getUDP())
		udpBox->setChecked(TRUE);
	
	tcpBox = new QCheckBox( netGroup, "tcpBox" );
	if (ihuconfig.getTCP())
		tcpBox->setChecked(TRUE);
	
	inportLabel = new QLabel( netGroup, "inportLabel" );
	
	inportBox = new QSpinBox( netGroup, "inportBox" );
	inportBox->setRange( 1, 32768 );
	inportBox->setValue( ihuconfig.getInPort() );
	
	nlay1->addStretch(1);
	nlay1->addWidget(inprotocolLabel);
	nlay1->addWidget(udpBox);
	nlay1->addWidget(tcpBox);
	nlay1->addStretch(2);
	nlay1->addWidget(inportLabel);
	nlay1->addWidget(inportBox);
	nlay1->addStretch(1);
	
	QBoxLayout *nouttopLayout = new QVBoxLayout( netOutGroup, 20);
	QBoxLayout *nlay2 = new QHBoxLayout( nouttopLayout, 10 );
	
	protocolLabel = new QLabel( netOutGroup, "protocolLabel" );
	
	udpRadioButton = new QRadioButton( tr("UDP"), netOutGroup, "udpRadioButton" );
	tcpRadioButton = new QRadioButton( tr("TCP"), netOutGroup, "tcpRadioButton" );
	
	if (ihuconfig.getProtocol())
		tcpRadioButton->setChecked(TRUE);
	else
		udpRadioButton->setChecked(TRUE);
	
	outportLabel = new QLabel( netOutGroup, "outportLabel" );
	
	outportBox = new QSpinBox( netOutGroup, "outportBox" );
	outportBox->setRange( 1, 32768 );
	outportBox->setValue( ihuconfig.getOutPort() );
	
	nlay2->addStretch(1);
	nlay2->addWidget(protocolLabel);
	nlay2->addWidget(udpRadioButton);
	nlay2->addWidget(tcpRadioButton);
	nlay2->addStretch(2);
	nlay2->addWidget(outportLabel);
	nlay2->addWidget(outportBox);
	nlay2->addStretch(1);
	
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
#endif
	dspBox->setCurrentItem(ihuconfig.getInputDriver());
	
	ringVolumeLabel = new QLabel( soundGroup, "ringVolumeLabel" );
	
	ringVolumeBox = new QComboBox( soundGroup, "ringVolumeBox" );
	ringVolumeBox->insertItem( tr( "Off" ), 0);
	ringVolumeBox->insertItem( tr( "High" ), 1);
	ringVolumeBox->insertItem( tr( "Medium" ), 2);
	ringVolumeBox->insertItem( tr( "Low" ), 3);
	ringVolumeBox->setCurrentItem( ihuconfig.getRingVolume() );
	
	slay1->addWidget(dspLabel);
	slay1->addWidget(dspBox, 1);
	slay1->addStretch(1);
	slay1->addWidget(ringVolumeLabel);
	slay1->addWidget(ringVolumeBox);
	
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
		QMessageBox::warning(0, "IHU Warning", "Warning: no soundcards found!");
	
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
	
	interfInBox->setEditText(ihuconfig.getInputInterface());
	interfOutBox->setEditText(ihuconfig.getOutputInterface());
	
	slay2->addWidget(interfInLabel);
	slay2->addWidget(interfInBox);
	slay2->addStretch();
	slay2->addWidget(interfOutLabel);
	slay2->addWidget(interfOutBox);
	
	modeLabel = new QLabel( soundGroup, "modeLabel" );
	
	modeBox = new QComboBox( soundGroup, "modeBox" );
	modeBox->insertItem( tr( "narrow (8000 Hz, 16 bit, Mono)" ), IHU_NARROW);
	modeBox->insertItem( tr( "wide (16000 Hz, 16 bit, Mono)" ), IHU_WIDE);
	modeBox->insertItem( tr( "ultra-wide (32000 Hz, 16 bit, Mono)" ), IHU_ULTRAWIDE);
	modeBox->setCurrentItem( ihuconfig.getSpeexMode() );
	
	slay3->addWidget(modeLabel);
	slay3->addWidget(modeBox, 1);
	
	stoptxLabel = new QLabel( soundGroup, "stoptxLabel" );
	
	stoptxBox = new QSpinBox( soundGroup, "stoptxBox" );
	stoptxBox->setRange( 0, 30 );
	stoptxBox->setValue( ihuconfig.getTXStop() );
	
	prepacketsLabel = new QLabel( soundGroup, "prepacketsLabel" );
	
	prepacketsBox = new QSpinBox( soundGroup, "prepacketsBox" );
	prepacketsBox->setRange( 1, 20 );
	prepacketsBox->setValue( ihuconfig.getPrePackets() );
	
	slay4->addWidget(stoptxLabel);
	slay4->addWidget(stoptxBox);
	slay4->addStretch(1);
	slay4->addWidget(prepacketsLabel);
	slay4->addWidget(prepacketsBox);
	
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
	bitrateBox->setCurrentItem( ihuconfig.getBitrateMode() );
	
	complexityLabel = new QLabel( encoderGroup, "complexityLabel" );
	
	complexityBox = new QSpinBox( encoderGroup, "complexityBox" );
	complexityBox->setRange( 1, 10 );
	complexityBox->setValue( ihuconfig.getComplexity() );
	
	elay1->addWidget(bitrateLabel);
	elay1->addWidget(bitrateBox, 1);
	elay1->addWidget(complexityLabel);
	elay1->addWidget(complexityBox);
	
	cbrqualityLabel = new QLabel( encoderGroup, "cbrqualityLabel" );
	
	cbrqualityBox = new QSpinBox( encoderGroup, "cbrqualityBox" );
	cbrqualityBox->setRange( 0, 10 );
	cbrqualityBox->setValue( ihuconfig.getQuality() );
	
	vbrqualityLabel = new QLabel( encoderGroup, "vbrqualityLabel" );
	
	vbrqualityBox = new QSpinBox( encoderGroup, "vbrqualityBox" );
	vbrqualityBox->setRange( 0, 10 );
	vbrqualityBox->setValue( ihuconfig.getVBRQuality() );
	
	abrLabel = new QLabel( encoderGroup, "abrLabel" );
	
	abrBox = new QSpinBox( encoderGroup, "abrBox" );
	abrBox->setRange( 2, 44 );
	abrBox->setValue( ihuconfig.getABR() );
	
	elay2->addWidget(cbrqualityLabel);
	elay2->addWidget(cbrqualityBox);
	elay2->addWidget(vbrqualityLabel);
	elay2->addWidget(vbrqualityBox);
	elay2->addWidget(abrLabel);
	elay2->addWidget(abrBox);
	
	vadBox = new QCheckBox( encoderGroup, "vadBox" );
	if (ihuconfig.getVAD())
		vadBox->setChecked(TRUE);
	
	dtxBox = new QCheckBox( encoderGroup, "dtxBox" );
	if (ihuconfig.getDTX())
		dtxBox->setChecked(TRUE);
	
	elay3->addWidget(vadBox);
	elay3->addStretch();
	elay3->addWidget(dtxBox);
	
	QBoxLayout *oLayout = new QVBoxLayout( optionsWidget, 10 );
	
	adrGroup = new QGroupBox( optionsWidget, "adrGroup" );
	adrGroup->setMargin( 0 );
	adrGroup->setMidLineWidth( 0 );
	
	oLayout->addWidget(adrGroup);
	
	agcGroup = new QGroupBox( optionsWidget, "agcGroup" );
	agcGroup->setMargin( 0 );
	agcGroup->setMidLineWidth( 0 );
	
	oLayout->addWidget(agcGroup);
	
	QBoxLayout *otopLayout = new QVBoxLayout( adrGroup, 20 );
	otopLayout->addSpacing(5);
	QBoxLayout *olay1 = new QHBoxLayout( otopLayout, 10 );
	QBoxLayout *olay2 = new QHBoxLayout( otopLayout, 10 );
	
	adrmindelayLabel = new QLabel( adrGroup, "adrmindelayLabel" );
	
	adrmindelayBox = new QSpinBox( adrGroup, "adrmindelayBox" );
	adrmindelayBox->setRange( 0, 9999 );
	adrmindelayBox->setValue( ihuconfig.getADRMinDelay() );
	
	adrmaxdelayLabel = new QLabel( adrGroup, "adrmaxdelayLabel" );
	
	adrmaxdelayBox = new QSpinBox( adrGroup, "adrmaxdelayBox" );
	adrmaxdelayBox->setRange( 50, 9999 );
	adrmaxdelayBox->setValue( ihuconfig.getADRMaxDelay() );
	
	adrstretchLabel = new QLabel( adrGroup, "adrstretchLabel" );
	
	adrstretchBox = new QSpinBox( adrGroup, "adrstretchBox" );
	adrstretchBox->setRange( 1, 50 );
	adrstretchBox->setValue( ihuconfig.getADRStretch() );
	
	olay1->addStretch();
	olay1->addWidget(adrstretchLabel);
	olay1->addWidget(adrstretchBox);
	olay1->addStretch();
	
	olay2->addWidget(adrmindelayLabel);
	olay2->addWidget(adrmindelayBox);
	olay2->addStretch();
	olay2->addWidget(adrmaxdelayLabel);
	olay2->addWidget(adrmaxdelayBox);
	
	QBoxLayout *ootopLayout = new QVBoxLayout( agcGroup, 20);
	ootopLayout->addSpacing(5);
	QBoxLayout *agclay1 = new QHBoxLayout( ootopLayout, 10 );
	
	agchwBox = new QCheckBox( agcGroup, "agchwBox" );
	
	mixerBox = new QComboBox( TRUE, agcGroup, "mixerBox" );
	mixerBox->setAutoCompletion(TRUE);
	mixerBox->setDuplicatesEnabled(FALSE);
	mixerBox->clear();
	mixerBox->insertItem( IHU_DEFAULT_AGC_CONTROL );
	mixerBox->insertItem( "AC97" );
	mixerBox->setEditText( ihuconfig.getAGCControl() );

	agcswBox = new QCheckBox( agcGroup, "agcswBox" );

	if (ihuconfig.getAGCHw())
	{
		agchwBox->setChecked(TRUE);
		mixerBox->setEnabled(TRUE);
	}
	else
	{
		agchwBox->setChecked(FALSE);
		mixerBox->setEnabled(FALSE);
	}

	if (ihuconfig.getAGCSw())
	{
		agcswBox->setChecked(TRUE);
	}
	else
	{
		agcswBox->setChecked(FALSE);
	}

	agclay1->addWidget(agchwBox);
	agclay1->addWidget(mixerBox);
	agclay1->addStretch();
	agclay1->addWidget(agcswBox);

	QGridLayout *gridLayout = new QGridLayout( ootopLayout, 2, 2, 5);

	agcStepLabel = new QLabel( agcGroup, "agcStepLabel" );
	
	agcstep = new QSlider( agcGroup, "agcstep" );
	agcstep->setOrientation( QSlider::Horizontal );
	agcstep->setRange( 1, 100 );
	agcstep->setTracking(TRUE);
	agcstep->setValue( ihuconfig.getAGCStep() );
	
	agcLevelLabel = new QLabel( agcGroup, "agcLevelLabel" );
	
	agclevel = new QSlider( agcGroup, "agclevel" );
	agclevel->setOrientation( QSlider::Horizontal );
	agclevel->setRange( -96, 0 );
	agclevel->setTracking(TRUE);
	agclevel->setValue( ihuconfig.getAGCLevel() );
	
	gridLayout->addWidget(agcLevelLabel, 0, 0, Qt::AlignBottom | Qt::AlignHCenter);
	gridLayout->addWidget(agclevel, 1, 0, Qt::AlignTop);
	gridLayout->addWidget(agcStepLabel, 0, 1, Qt::AlignBottom | Qt::AlignHCenter);
	gridLayout->addWidget(agcstep, 1, 1, Qt::AlignTop);
	
	QBoxLayout *kLayout = new QVBoxLayout( securityWidget, 10 );
	
	securityGroup = new QButtonGroup( securityWidget, "securityGroup" );
	securityGroup->setRadioButtonExclusive(TRUE);
	securityGroup->setMargin( 0 );
	securityGroup->setMidLineWidth( 0 );
	
	kLayout->addWidget(securityGroup);
	
	QBoxLayout *ktopLayout = new QVBoxLayout( securityGroup, 20, 10 );
	
	QBoxLayout *klay0 = new QHBoxLayout( ktopLayout, 10 );
	QBoxLayout *klay1 = new QHBoxLayout( ktopLayout, 10 );
	QBoxLayout *klay2 = new QHBoxLayout( ktopLayout, 10 );
	QBoxLayout *klay3 = new QHBoxLayout( ktopLayout, 10 );
	
	cryptBox = new QCheckBox( securityGroup, "cryptBox" );
	if (ihuconfig.getCrypt())
		cryptBox->setChecked(TRUE);
	klay0->addWidget(cryptBox);

	keyLabel = new QLabel( securityGroup, "keyLabel" );
	
	randomRadioButton = new QRadioButton( tr(""), securityGroup, "randomRadioButton" );
	
	bitsBox = new QSpinBox( securityGroup, "bitsBox" );
	bitsBox->setLineStep( 8 );
	bitsBox->setRange( 8, 448 );
	bitsBox->setValue( ihuconfig.getKeyLen() );
	
	passwordRadioButton = new QRadioButton( tr(""), securityGroup, "passwordRadioButton" );
	
	if (ihuconfig.getRandom())
		randomRadioButton->setChecked(TRUE);
	else
		passwordRadioButton->setChecked(TRUE);
	
	passButton = new QPushButton( securityGroup, "passButton" );

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
		kkklay2->addWidget(passButton);
	
	showkeyBox = new QCheckBox( securityGroup, "showkeyBox" );
	if (ihuconfig.getShowKey())
		showkeyBox->setChecked(TRUE);
	
	klay2->addWidget(showkeyBox);
	
	logFile = ihuconfig.getLogFile();
	
	logBox = new QCheckBox( securityGroup, "logBox" );
	if (!logFile.isEmpty())
		logBox->setChecked(TRUE);	

	logNameLabel = new QLabel( securityGroup, "logNameLabel" );
	logNameLabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
	logNameLabel->setMinimumWidth(300);
	logNameLabel->setText(fileName(logFile));

	klay3->addWidget(logBox);
	klay3->addWidget(logNameLabel);
	
	languageChange();
	bitrateChanged();
	
	addTab(generalWidget, tr("&General"));
	addTab(netWidget, tr("&Network"));
	addTab(soundWidget, tr("&Sound"));
	addTab(encoderWidget, "&Encoder");
	addTab(optionsWidget, "&Options");
	addTab(securityWidget, tr("Se&curity"));
	
	connect( this, SIGNAL( applyButtonPressed() ), this, SLOT( saveAndClose() ) );
	connect( this, SIGNAL( defaultButtonPressed() ), this, SLOT( defaults() ) );
	connect( this, SIGNAL( helpButtonPressed() ), this, SLOT( saveToFile() ) );
	connect( bitrateBox, SIGNAL( activated(int) ), this, SLOT( bitrateChanged() ) );
	connect( logBox, SIGNAL( clicked() ), this, SLOT( logEnable() ) );
	connect( trayBox, SIGNAL( toggled(bool) ), this, SLOT( minimizeEnable(bool) ) );
	connect( agclevel, SIGNAL( valueChanged(int) ), this, SLOT( agcLevelChanged(int) ) );
	connect( agcstep, SIGNAL( valueChanged(int) ), this, SLOT( agcStepChanged(int) ) );
	connect( agchwBox, SIGNAL( toggled(bool) ), this, SLOT( agcHwToggled(bool) ) );
	connect( passButton, SIGNAL( clicked() ), this, SLOT( passButtonClicked() ) );
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
	maxCallLabel->setText( tr( "Max Call Number" ) );
	maxHostLabel->setText( tr( "Save Host History" ) );
	inprotocolLabel->setText( tr( "Allowed protocols:" ) );
	protocolLabel->setText( tr( "Preferred protocol:" ) );
	udpBox->setText( tr( "UDP" ) );
	tcpBox->setText( tr( "TCP" ) );
	inportLabel->setText( tr( "Port:" ) );
	outportLabel->setText( tr( "Default port:" ) );
	dspLabel->setText( tr( "Driver" ) );
	interfInLabel->setText( tr( "Input interface" ) );
	interfOutLabel->setText( tr( "Output interface" ) );
	stoptxLabel->setText( tr( "Stop TX after (silence secs)" ) );
	ringVolumeLabel->setText( tr( "Ring volume" ) );
	prepacketsLabel->setText( tr( "Packets prebuffer" ) );
	adrmindelayLabel->setText( tr( "Minimum delay (ms)" ) );
	adrmaxdelayLabel->setText( tr( "Maximum delay (ms)" ) );
	adrstretchLabel->setText( tr( "Time change percentage (0-50 \%)") );
	agcLevelLabel->setText( QString( "Volume Level: %1 dB" ).arg(agclevel->value()) );
	agcStepLabel->setText( QString( "Volume change speed: %1" ).arg(agcstep->value()) );
	modeLabel->setText( tr( "Input format" ) );
	bitrateLabel->setText( tr( "Bitrate mode" ) );
	complexityLabel->setText( tr( "Complexity" ) );
	cbrqualityLabel->setText( tr( "CBR Quality" ) );
	abrLabel->setText( tr( "ABR kbps" ) );
	vbrqualityLabel->setText( tr( "VBR Quality" ) );
	vadBox->setText( tr( "Voice Activity Detection (VAD)" ) );
	dtxBox->setText( tr( "Discontinuous TX" ) );
	agchwBox->setText( tr( "ALSA Mixer Control:" ) );
	agcswBox->setText( tr( "IHU Internal Control" ) );
	cryptBox->setText( tr( "Always encrypt outgoing stream" ) );
	showkeyBox->setText( tr( "Show decryption key of incoming stream in the Log" ) );
	passwordRadioButton->setText( tr( "Text passphrase (max 56 chars)" ) );
	randomRadioButton->setText( tr( "Random with constant length (bits):" ) );
	keyLabel->setText( tr( "Encryption key type:" ) );
	passButton->setText( tr( "Save Passphrase"));
	logBox->setText( tr( "Log file"));

	setOkButton(QString::null);
	setHelpButton("Save to file...");
	setApplyButton("OK");
	setCancelButton();
	setDefaultButton();
	
	QWhatsThis::add(nameEdit, tr("Your name, nickname, address, or whatever you want that your partner will see in order to recognize you. This information will be shown only to the person who's talking with you (leave empty to be anonymous)."));
	QWhatsThis::add(answerBox, tr("When you check this option, IHU will automatically answer the incoming calls, without need to click on Answer button."));
	QWhatsThis::add(trayBox, tr("Enable/disable the IHU icon in the system tray."));
	QWhatsThis::add(waitBox, tr("To wait for calls immediately when you launch IHU, without need to click on Wait for calls button."));
	QWhatsThis::add(hideBox, tr("This will hide the main window and will automatically minimize IHU to system tray at startup."));
	QWhatsThis::add(maxCallBox, tr("This is the maximum number of calls per session."));
	QWhatsThis::add(maxHostBox, tr("This is the number of addresses saved from the host history."));
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
	QWhatsThis::add(prepacketsBox, tr("Number of packets to prebuffer before starting to play. A high number of packets ensure audio continuity, but it could increase the audio delay."));
	QWhatsThis::add(adrmindelayBox, tr("This is the minimum audio delay on the output sound interface. When ADR is activated, if the delay is smaller than maximum, IHU will try to increase the delay to this value in order to avoid sound crackle."));
	QWhatsThis::add(adrmaxdelayBox, tr("This is the maximum audio delay on the output sound interface. When ADR is activated, if the delay is greater than minimum, IHU will try to reduce the delay to this value."));
	QWhatsThis::add(adrstretchBox, tr("This is the time change percentage for ADR. If the current playback delay is smaller/greater than reference values, than IHU will adjust the current audio stream. A higher time stretch means faster adjustment."));
	QWhatsThis::add(agcstep, tr("This value will influence the rapidity of AGC volume adjustment."));
	QWhatsThis::add(bitrateBox, tr("Choose the encoding bitrate type for your voice beetween: Constant (CBR), Variable (VBR), Average (ABR)."));
	QWhatsThis::add(complexityBox, tr("Algorithmic quality. 0 means less CPU computation but lower quality, 10 means highest quality but more computation."));
	QWhatsThis::add(cbrqualityBox, tr("CBR quality: 0 lowest bitrate, 10 highest bitrate."));
	QWhatsThis::add(vbrqualityBox, tr("VBR quality: 0 lowest bitrate, 10 highest bitrate."));
	QWhatsThis::add(abrBox, tr("ABR bitrate (kilobit per second). You can specify your preferred bitrate average value."));
	QWhatsThis::add(vadBox, tr("When enabled, it detects whether the audio being encoded is speech or silence/background noise. VAD is implicitly activated when encoding is VBR."));
	QWhatsThis::add(dtxBox, tr("It's an addition to VAD/VBR operation, that allows to stop transmitting completely when background noise is stationary."));
	QWhatsThis::add(adrGroup, tr("The ADR option is useful to reduce the audio delay that could build up in the player due to network delay."));
	QWhatsThis::add(agcGroup, tr("The AGC option is useful to automatically regulate or to maintain stationary the volume level of your recorded voice."));
	QWhatsThis::add(agchwBox, tr("The ALSA Mixer Control method will automatically adjust the specified mixer control to reach the volume level. Disable this if you have problems or if you don't want IHU to change your mixer settings."));
	QWhatsThis::add(agcswBox, tr("The IHU Internal Control will amplify or soften the recorded samples to reach the volume level, but no mixer level will be touched."));
	QWhatsThis::add(cryptBox, tr("When you check this option, IHU will automatically encrypt the outgoing stream of the call."));
	QWhatsThis::add(randomRadioButton, tr("IHU will automatically choose a new random key."));
	QWhatsThis::add(passwordRadioButton, tr("IHU will ask you a passphrase when you enable encryption or when you change the encryption key."));
	QWhatsThis::add(bitsBox, tr("Key length (in bits) of random key."));
	QWhatsThis::add(showkeyBox, tr("This option will show the decryption passphrase of the incoming stream in the Log. Be careful, this might show confidential informations!"));
	QWhatsThis::add(logBox, tr("Write a copy of the log to file."));
}

void Settings::saveAndClose()
{
	if (saveSettings())
		accept();
}

bool Settings::saveSettings()
{
	bool ret = false;
	try
	{
		if (dtxBox->isChecked() && !(vadBox->isChecked() || (bitrateBox->currentItem())))
			QMessageBox::warning(0, "IHU Warning", "Warning: Discontinuos TX is useless without VAD, VBR or ABR.");
		
		if (inportBox->value() < 1025)
			QMessageBox::warning(0, "IHU Warning", "Warning: you must be root to use a port lower than 1025.");
		
		if (dspBox->currentItem() > 0)
			QMessageBox::warning(0, "IHU Warning", "Warning: you chose to use Jack as input sound driver.\nPlease ensure that Jack is running with correct settings.");
		
		if ((adrmaxdelayBox->value() - adrmindelayBox->value()) < 50)
			QMessageBox::warning(0, "IHU Warning", "Warning: the ADR Min Delay value is too near to Max Delay.\nThe ADR Option might not work properly.");

		if (!udpBox->isChecked() && !tcpBox->isChecked())
			throw Error("no Receiver protocol was selected.\nPlease choose at least one protocol.");
		
		if (!agchwBox->isChecked() && !agcswBox->isChecked())
			throw Error("no AGC Control selected.\nPlease choose at least one method.");

		if (adrmaxdelayBox->value() < adrmindelayBox->value())
			throw Error("ADR Max delay is smaller than Min delay!");
		
		ihuconfig.setMyName(nameEdit->text());
		ihuconfig.setAutoAnswer(answerBox->isChecked());
		ihuconfig.setTrayIcon(trayBox->isChecked());
		ihuconfig.setAutoWait(waitBox->isChecked());
		ihuconfig.setAutoHide(hideBox->isChecked());
		ihuconfig.setMaxCalls(maxCallBox->value());
		ihuconfig.setMaxHosts(maxHostBox->value());
	
		ihuconfig.setUDP(udpBox->isChecked());
		ihuconfig.setTCP(tcpBox->isChecked());
		ihuconfig.setInPort(inportBox->value());
		if (tcpRadioButton->isChecked())
			ihuconfig.setProtocol(IHU_TCP);
		else
			ihuconfig.setProtocol(IHU_UDP);
		ihuconfig.setOutPort(outportBox->value());
		
		switch(dspBox->currentItem())
		{
			case 0:
				ihuconfig.setInputDriver(0);
				break;
			case 1:
				ihuconfig.setInputDriver(1);
				break;
		}
		ihuconfig.setInputInterface(interfInBox->currentText());
		ihuconfig.setOutputInterface(interfOutBox->currentText());
		ihuconfig.setTXStop(stoptxBox->value());
		ihuconfig.setRingVolume(ringVolumeBox->currentItem());
		ihuconfig.setPrePackets(prepacketsBox->value());
		ihuconfig.setAGCLevel(agclevel->value());
		ihuconfig.setAGCStep(agcstep->value());
		ihuconfig.setAGCHw(agchwBox->isChecked());
		ihuconfig.setAGCSw(agcswBox->isChecked());
		ihuconfig.setAGCControl(mixerBox->currentText());
		ihuconfig.setADRMinDelay(adrmindelayBox->value());
		ihuconfig.setADRMaxDelay(adrmaxdelayBox->value());
		ihuconfig.setADRStretch(adrstretchBox->value());
		
		ihuconfig.setComplexity(complexityBox->value());
		ihuconfig.setQuality(cbrqualityBox->value());
		ihuconfig.setVBRQuality(vbrqualityBox->value());
		ihuconfig.setSpeexMode(modeBox->currentItem());
		ihuconfig.setBitrateMode(bitrateBox->currentItem());
		ihuconfig.setABR(abrBox->value());
		ihuconfig.setVAD(vadBox->isChecked());
		ihuconfig.setDTX(dtxBox->isChecked());
		
		ihuconfig.setCrypt(cryptBox->isChecked());
		ihuconfig.setRandom(randomRadioButton->isChecked());
		ihuconfig.setKeyLen(bitsBox->value());
		ihuconfig.setShowKey(showkeyBox->isChecked());
		ihuconfig.setLogFile(logFile);
		
		ret = true;
	}
	catch (Error e)
	{
		QMessageBox::critical(0, "IHU Settings Error", "Error: " + e.getText());
	}
	return ret;
}

void Settings::defaults()
{
	QWidget *current = currentPage();
	if (current == generalWidget)
	{
		answerBox->setChecked(IHU_DEFAULT_ANSWER);
		trayBox->setChecked(IHU_DEFAULT_TRAY);
		waitBox->setChecked(IHU_DEFAULT_WAIT);
		hideBox->setChecked(IHU_DEFAULT_HIDE);
		maxCallBox->setValue(IHU_DEFAULT_MAXCALLS);
		maxHostBox->setValue(IHU_DEFAULT_MAXHOSTS);
	}
	else
	if (current == netWidget)
	{
		inportBox->setValue(IHU_DEFAULT_INPORT);
		outportBox->setValue(IHU_DEFAULT_OUTPORT);
		udpBox->setChecked(IHU_DEFAULT_UDP);
		tcpBox->setChecked(IHU_DEFAULT_TCP);
		udpRadioButton->setChecked(TRUE);
	}
	else
	if (current == soundWidget)
	{
		modeBox->setCurrentItem(IHU_DEFAULT_MODE);
		stoptxBox->setValue(IHU_DEFAULT_TXSTOP);
		ringVolumeBox->setCurrentItem(IHU_DEFAULT_RINGVOLUME);
		prepacketsBox->setValue(IHU_DEFAULT_PREPACKETS);
		dspBox->setCurrentItem(IHU_DEFAULT_INDRIVER);
		interfInBox->setCurrentText(IHU_DEFAULT_INTERFACE);
		interfOutBox->setCurrentText(IHU_DEFAULT_INTERFACE);
	}
	else
	if (current == encoderWidget)
	{
		bitrateBox->setCurrentItem(IHU_DEFAULT_BITRATEMODE);
		complexityBox->setValue(IHU_DEFAULT_COMPLEXITY);
		cbrqualityBox->setValue(IHU_DEFAULT_QUALITY);
		vbrqualityBox->setValue(IHU_DEFAULT_VBRQUALITY);
		abrBox->setValue(IHU_DEFAULT_ABR);
		vadBox->setChecked(IHU_DEFAULT_VAD);
		dtxBox->setChecked(IHU_DEFAULT_DTX);
		bitrateChanged();
	}
	else
	if (current == optionsWidget)
	{
		agcstep->setValue(IHU_DEFAULT_AGCSTEP);
		agclevel->setValue(IHU_DEFAULT_AGCLEVEL);
		adrmindelayBox->setValue(IHU_DEFAULT_ADRMINDELAY);
		adrmaxdelayBox->setValue(IHU_DEFAULT_ADRMAXDELAY);
		adrstretchBox->setValue(IHU_DEFAULT_ADRSTRETCH);
		agchwBox->setChecked(IHU_DEFAULT_AGC_HW);
		agcswBox->setChecked(IHU_DEFAULT_AGC_SW);
		mixerBox->setEditText( IHU_DEFAULT_AGC_CONTROL );
	}
	else
	if (current == securityWidget)
	{
		cryptBox->setChecked(FALSE);
		bitsBox->setValue(IHU_DEFAULT_KEYLEN);
		randomRadioButton->setChecked(IHU_DEFAULT_RANDOM);
		showkeyBox->setChecked(IHU_DEFAULT_SHOWKEY);
	}

}

void Settings::logEnable()
{
	if (logBox->isOn())
	{
		QString name=QFileDialog::getSaveFileName(ihuconfig.getLogFile(),"*.*", this, 0, "Save to file...");
		if (!name.isEmpty())
			logFile = name;
		else
			logBox->setChecked(FALSE);
	}
	else
	{
		logFile = "";
	}
	logNameLabel->setText(fileName(logFile));
}

void Settings::bitrateChanged()
{
    switch(bitrateBox->currentItem())
    {
	case IHU_NARROW:
		cbrqualityBox->setEnabled(TRUE);
		vbrqualityBox->setEnabled(FALSE);
		abrBox->setEnabled(FALSE);
		break;
	case IHU_WIDE:
		cbrqualityBox->setEnabled(FALSE);
		vbrqualityBox->setEnabled(TRUE);
		abrBox->setEnabled(FALSE);
		break;
	case IHU_ULTRAWIDE:
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

void Settings::agcLevelChanged(int value)
{
	agcLevelLabel->setText( QString( "Volume Level: %1 dB" ).arg(agclevel->value()) );
}

void Settings::agcStepChanged(int value)
{
	agcStepLabel->setText( QString( "Volume change speed: %1" ).arg(agcstep->value()) );
}

void Settings::agcHwToggled(bool on)
{
	mixerBox->setEnabled(on);
}

void Settings::saveToFile()
{
	if(saveSettings())
	{
		QString name = QFileDialog::getSaveFileName(ihuconfig.getFileName(),"*.xml", this, 0, "Save config to file...");
		if (!name.isEmpty())
		{
			try
			{
				ihuconfig.writeConfig(name);
			}
			catch (Error e)
			{
				QMessageBox::critical(0, "IHU Settings Error", "Error: " + e.getText());
			}
		}	
	}
}

void Settings::passButtonClicked()
{
	bool ok;
	QString text = QInputDialog::getText(QString("Encryption passphrase"), QString("IMPORTANT: THE PASSPHRASE IS SAVED AS CLEARTEXT IN THE CONFIG FILE!\nIf you don't want to save the passphrase, you can always use the main window to set it when needed.\nEnter the encryption passphrase (max 56 chars, please use at least 30 chars, leave blank to reset):"), QLineEdit::Password, ihuconfig.getPasswd(), &ok, this );
	if (ok)
	{
		ihuconfig.setPasswd(text);
	}
}
