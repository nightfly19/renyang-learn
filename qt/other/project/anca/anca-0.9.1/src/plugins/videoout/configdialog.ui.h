/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

#include "ancaconf.h"

void ConfigDialog::init()
{
	bool scaled = ancaConf->readBoolEntry("videoout", "scaled", true);
	bool clipped = ancaConf->readBoolEntry("videoout", "clipped", true);
	
	if( scaled )
		buttonGroup->setButton(0);
	else if( clipped )
		buttonGroup->setButton(1);
	else
		buttonGroup->setButton(2);
}

void ConfigDialog::buttonGroup_clicked( int index )
{
	switch( index ) {
	case 0:
		ancaConf->writeBoolEntry("videoout", "scaled", true);
		ancaConf->writeBoolEntry("videoout", "clipped", true);
		break;
	case 1:
		ancaConf->writeBoolEntry("videoout", "scaled", false);
		ancaConf->writeBoolEntry("videoout", "clipped", true);
		break;
	case 2:
		ancaConf->writeBoolEntry("videoout", "scaled", false);
		ancaConf->writeBoolEntry("videoout", "clipped", false);
	default:
		break;
	}
}
