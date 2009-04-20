

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

#ifndef SETTINGS_H
#define SETTINGS_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qvariant.h>
#include <qtabdialog.h>

#include "Config.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QGroupBox;
class QComboBox;
class QRadioButton;
class QButtonGroup;

class Settings : public QTabDialog
{
	Q_OBJECT

public:
	Settings( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
	~Settings();
	
	QWidget* generalWidget;
	QWidget* netWidget;
	QWidget* soundWidget;
	QWidget* encoderWidget;
	QWidget* optionsWidget;
	QWidget* securityWidget;
	QGroupBox* generalGroup;
	QButtonGroup* netGroup;
	QButtonGroup* netOutGroup;
	QButtonGroup* netInGroup;
	QGroupBox* soundGroup;
	QGroupBox* encoderGroup;
	QGroupBox* adrGroup;
	QGroupBox* agcGroup;
	QButtonGroup* securityGroup;
	QLineEdit *nameEdit;
	QLabel* nameLabel;
	QLabel* dumpLabel;
	QCheckBox* answerBox;
	QCheckBox* trayBox;
	QCheckBox* waitBox;
	QCheckBox* hideBox;

	QRadioButton* udpBox;
	QRadioButton* tcpBox;
	QRadioButton* sctpBox;
	QRadioButton* sctp_udpBox;

	QLabel* dspLabel;
	QLabel* interfInLabel;
	QLabel* interfOutLabel;
	QLabel* modeLabel;
	QLabel* bitrateLabel;
	QLabel* complexityLabel;
	QLabel* cbrqualityLabel;
	QLabel* vbrqualityLabel;
	QLabel* abrLabel;
	QLabel* adrmaxdelayLabel;
	QLabel* adrmindelayLabel;
	QLabel* adrstretchLabel;
	QLabel* agcSpeedLabel;
	QLabel* agcminLabel;
	QLabel* agcmaxLabel;
	QLabel* protocolLabel;
	QLabel* inprotocolLabel;
	QLabel* inportLabel;
	QLabel* outportLabel;
	QLabel* stoptxLabel;
	QLabel* ringVolumeLabel;
	QSpinBox *stoptxBox;
	QSpinBox *agcBox;
	QSpinBox *agcstepBox;
	QSpinBox *agcminBox;
	QSpinBox *agcmaxBox;
	QSpinBox* adrmindelayBox;
	QSpinBox* adrmaxdelayBox;
	QSpinBox *adrstretchBox;
	QSpinBox* outportBox;
	QSpinBox* inportBox;
	QSpinBox* complexityBox;
	QSpinBox* cbrqualityBox;
	QSpinBox* abrBox;
	QSpinBox* vbrqualityBox;
	QPushButton* openInButton;
	QPushButton* openOutButton;
	QCheckBox* vadBox;
	QCheckBox* dtxBox;
	QComboBox *dspBox;
	QComboBox *interfInBox;
	QComboBox *interfOutBox;
	QComboBox *modeBox;
	QComboBox *bitrateBox;
	QComboBox *ringVolumeBox;
	QRadioButton* udpRadioButton;
	QRadioButton* tcpRadioButton;
	QRadioButton* sctpRadioButton;
	QRadioButton* sctp_udpRadioButton;
	QRadioButton* randomRadioButton;
	QRadioButton* passwordRadioButton;
	QButtonGroup* securityButtonGroup;
	QLabel* keyLabel;
	QSpinBox* bitsBox;
	QLabel* logLabel;
	QPushButton* logButton;
	QCheckBox* showkeyBox;
    
private:
	QString inFile;
	QString outFile;
	QString logFile;
    
protected:
	Config &config;

public slots:
	virtual void languageChange();
	virtual void saveSettings();
	virtual void defaults();
	virtual void logEnable(bool);
	virtual void dumpinEnable(bool);
	virtual void dumpoutEnable(bool);
	virtual void bitrateChanged();
	virtual void minimizeEnable(bool);
	virtual QString fileName(QString);
};

#endif // SETTINGS_H
