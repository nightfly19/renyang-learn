/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

#include "ancaconf.h"
#include <qfiledialog.h>
#include "videograbber.h"
#include <ptlib.h>

void ConfigDialog::init()
{
	QString videoDevice = ancaConf->readEntry( "videoin", "videoDevice", (const char *)PVideoChannel::GetDefaultDevice(PVideoChannel::Recorder) );
	int videoChannel = ancaConf->readIntEntry( "videoin", "videoChannel", -1 );
	int videoFormat = ancaConf->readIntEntry( "videoin", "videoFormat", PVideoDevice::PAL );
	int videoSize = ancaConf->readIntEntry( "videoin", "videoSize", VideoGrabber::V_LARGE );
	bool previewSize = ancaConf->readBoolEntry( "videoin", "previewShowCallSize", true );
	QString videoImage = ancaConf->readEntry( "videoin", "videoImage", QString::null );

	formatComboBox->setCurrentItem(videoFormat);
	channelSpinBox->setValue(videoChannel);
	sizeComboBox->setCurrentItem(videoSize);
	callSizeCheckBox->setChecked(previewSize);
	pictureEdit->setText( videoImage );

	PVideoInputDevice device;
	PStringArray names = device.GetDeviceNames();
	for (PINDEX i = 0; i < names.GetSize(); i++) {
		deviceComboBox->insertItem( (const char *)names[i] );
	}

	// test if configuration setting is valid for now
	bool valid = false;
	for (int i = 0; i < deviceComboBox->count(); i++)
		if( videoDevice == deviceComboBox->text(i) ) {
			deviceComboBox->setCurrentItem(i);
			valid = true;
			break;
		}
	if( valid )
		applyButton->setEnabled(false);
}

void ConfigDialog::okButton_clicked()
{
	applyButton_clicked();

	accept();
}

void ConfigDialog::applyButton_clicked()
{
	QString videoDevice = ancaConf->readEntry( "videoin", "videoDevice", (const char *)PVideoChannel::GetDefaultDevice(PVideoChannel::Recorder) );
	int videoChannel = ancaConf->readIntEntry( "videoin", "videoChannel", -1 );
	int videoFormat = ancaConf->readIntEntry( "videoin", "videoFormat", PVideoDevice::PAL );
	int videoSize = ancaConf->readIntEntry( "videoin", "videoSize", VideoGrabber::V_LARGE );
	bool previewSize = ancaConf->readBoolEntry( "videoin", "previewShowCallSize", true );
	QString videoImage = ancaConf->readEntry( "videoin", "videoImage", QString::null );

	if( videoDevice != deviceComboBox->currentText() ||
		videoChannel != channelSpinBox->value() ||
		videoFormat != formatComboBox->currentItem() ||
		videoSize != sizeComboBox->currentItem() ||
		previewSize != callSizeCheckBox->isChecked() ||
		videoImage != pictureEdit->text() ) {

		ancaConf->writeEntry( "videoin", "videoDevice", deviceComboBox->currentText() );
		ancaConf->writeIntEntry( "videoin", "videoChannel", channelSpinBox->value() );
		ancaConf->writeIntEntry( "videoin", "videoFormat", formatComboBox->currentItem() );
		ancaConf->writeIntEntry( "videoin", "videoSize", sizeComboBox->currentItem() );
		ancaConf->writeBoolEntry( "videoin", "previewShowCallSize", callSizeCheckBox->isChecked() );
		ancaConf->writeEntry( "videoin", "videoImage", pictureEdit->text() );
		
		videoGrabber->configurationChanged();
	}

	applyButton->setEnabled(false);
}

void ConfigDialog::settingsChanged()
{
	applyButton->setEnabled(true);
}


void ConfigDialog::openButton_clicked()
{
	
	QString s = QFileDialog::getOpenFileName(
			QString::null,
			"All Images (*.bmp *.jpg *.jpeg *.pbm *.pgm *.png *.ppm *.xbm *.xpm)",
			this,
			"open file dialog"
			"Choose a picture" );

	if( !s.isEmpty() ) {
		pictureEdit->setText(s);
	}
}
