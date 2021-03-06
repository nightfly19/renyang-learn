/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/

#include "ancaconf.h"
#include "stdconf.h"

#include <ptlib.h>

void ConfigDialog::init() {
	QString player = ancaConf->readEntry( "audio", "audioPlayerDevice", (const char *)PSoundChannel::GetDefaultDevice(PSoundChannel::Player) );
	QString recorder = ancaConf->readEntry( "audio", "audioRecorderDevice", (const char *)PSoundChannel::GetDefaultDevice(PSoundChannel::Recorder) );
	
	PStringArray playerNames = PSoundChannel::GetDeviceNames(PSoundChannel::Player);
	for (PINDEX i = 0; i < playerNames.GetSize(); i++)
		playComboBox->insertItem( (const char *)playerNames[i] );

	PStringArray recorderNames = PSoundChannel::GetDeviceNames(PSoundChannel::Recorder);
	for (PINDEX i = 0; i < recorderNames.GetSize(); i++)
		recComboBox->insertItem( (const char *)recorderNames[i] );

	// test if configuration setting is valid for now
	bool valid1 = false;
	for (int i = 0; i < playComboBox->count(); i++)
		if( player == playComboBox->text(i) ) {
			playComboBox->setCurrentItem(i);
			valid1 = true;
			break;
		}
	bool valid2 = false;
	for (int i = 0; i < recComboBox->count(); i++)
		if( recorder == recComboBox->text(i) ) {
			recComboBox->setCurrentItem(i);
			valid2 = true;
			break;
		}
	if( valid1 && valid2 )
		applyButton->setEnabled(false);
}

void ConfigDialog::defaultButton_clicked()
{
	apply = false;
}


void ConfigDialog::applyButton_clicked()
{
	applyChanges();
}


void ConfigDialog::okButton_clicked()
{
	applyChanges();
	accept();
}


void ConfigDialog::settingsChanged()
{
	applyButton->setEnabled(true);
	apply = true;
}


void ConfigDialog::applyChanges()
{
	if( !apply ) return;
	
	QString recorderDevice = recComboBox->currentText();
	QString playerDevice = playComboBox->currentText();
	
	ancaConf->writeEntry( "audio", "audioPlayerDevice", playerDevice );
	ancaConf->writeEntry( "audio", "audioRecorderDevice", recorderDevice );
	
	apply = false;
	applyButton->setEnabled(false);
}
