/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

#include "ancaconf.h"
#include "stdconf.h"
#include "addressbook.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qdir.h>
#include <qfileinfo.h>
#include <qfiledialog.h>

void EditDialog::init()
{
	pictureComboBox->insertItem( "no picture" );

	QStringList pictures = ancaConf->readListEntry( ANCA_PICTURES_DIR, ANCA_PICTURES_DIR_DEFAULT );
#ifdef PREL
	pictures.append( PREL "/share/anca/pictures");
#endif
	//append home dir
	pictures.append( "./anca/pictures");

	for( QStringList::Iterator it = pictures.begin(); it != pictures.end(); ++it ) {
		QDir dir( *it );
		if( dir.cd("addressbook") ) {
			dir.setFilter( QDir::Files );
			const QFileInfoList *list = dir.entryInfoList();
			QFileInfoListIterator it( *list );
			QFileInfo *fi;
			
			while ( (fi = it.current()) ) {
				QPixmap p(fi->filePath());
				if( !p.isNull() )
					pictureComboBox->insertItem( p, fi->fileName() );
				++it;
			}
		}
	}

	pictureComboBox->insertItem( QPixmap::fromMimeSource("defaultPicture.png"), "default");
	pictureComboBox->setCurrentItem( pictureComboBox->count() - 1 );
	
	m_picture = "default";
}


void EditDialog::pictureChanged( const QString & picture )
{
	if( picture == "no picture" ) {
		picturePixmapLabel->setPixmap( QPixmap::fromMimeSource("nonePicture.png") );
		m_picture = QString::null;
	}
	else if( picture == "default" ) {
		picturePixmapLabel->setPixmap( QPixmap::fromMimeSource("defaultPicture.png") );
		m_picture = "default";
	}
	else {
		QStringList pictures = ancaConf->readListEntry( ANCA_PICTURES_DIR, ANCA_PICTURES_DIR_DEFAULT );
#ifdef PREL
		pictures.append( PREL "/share/anca/pictures");
#endif
		pictures.append( "./anca/pictures");

		for( QStringList::Iterator it = pictures.begin(); it != pictures.end(); ++it ) {
			QDir dir( *it );
			if( dir.cd("addressbook") ) {
				QFileInfo fi( dir, picture );
				picturePixmapLabel->setPixmap( QPixmap( fi.filePath() ) );
				m_picture = fi.filePath();
			}
		}
	}


}

QString& EditDialog::picture()
{
	return m_picture;
}

void EditDialog::openPicture()
{
	QString s = QFileDialog::getOpenFileName(
			QString::null,
			"All Images (*.bmp *.jpg *.jpeg *.pbm *.pgm *.png *.ppm *.xbm *.xpm)",
			this,
			"open file dialog"
			"Choose a picture" );
	QPixmap p(s);
	if( !p.isNull() ) {
//		pictureComboBox->insertItem( p, s );
//		pictureComboBox->setCurrentItem( pictureComboBox->count() - 1 );
		picturePixmapLabel->setPixmap( p );
		m_picture = s;
	}
}


void EditDialog::setPicture( const QString & picture )
{
	if( picture.isNull() ) {
		picturePixmapLabel->setPixmap( QPixmap::fromMimeSource("nonePicture.png") );
		m_picture = QString::null;
	}
	else if( picture == "default" ) {
		picturePixmapLabel->setPixmap( QPixmap::fromMimeSource("defaultPicture.png") );
		m_picture = "default";
	}
	else {
		picturePixmapLabel->setPixmap( QPixmap(picture) );
		m_picture = picture;
	}


}

void EditDialog::setItem( AdbItem *item )
{
	if( !item ) return;

	if( item->bookmarked() )
		setCaption( "Edit " + item->nick() + " -- Anca" );
	else
		setCaption( "Save " + item->nick() + " -- Anca" );

	urlEdit->setReadOnly(item->inLDAP());
	nickEdit->setReadOnly(item->inLDAP());
	firstNameEdit->setReadOnly(item->inLDAP());
	lastNameEdit->setReadOnly(item->inLDAP());
	mailEdit->setReadOnly(item->inLDAP());
	phoneEdit->setReadOnly(item->inLDAP());
	mobileEdit->setReadOnly(item->inLDAP());
	streetEdit->setReadOnly(item->inLDAP());
	cityEdit->setReadOnly(item->inLDAP());
	codeEdit->setReadOnly(item->inLDAP());
	stateEdit->setReadOnly(item->inLDAP());
	orgEdit->setReadOnly(item->inLDAP());
	posEdit->setReadOnly(item->inLDAP());
	typeEdit->setReadOnly(item->inLDAP());
	roomEdit->setReadOnly(item->inLDAP());

	urlEdit->setText( item->url() );
	nickEdit->setText( item->nick() );
	firstNameEdit->setText( item->givenName() );
	lastNameEdit->setText( item->surname() );
	mailEdit->setText( item->email() );
	phoneEdit->setText( item->telephone() );
	mobileEdit->setText( item->mobile() );
	streetEdit->setText( item->street() );
	cityEdit->setText( item->city() );
	codeEdit->setText( item->code() );
	stateEdit->setText( item->state() );
	orgEdit->setText( item->organization() );
	posEdit->setText( item->employeeNumber() );
	typeEdit->setText( item->employeeType() );
	roomEdit->setText( item->departmentNumber() );

	setPicture( item->picture() );
}

void EditDialog::creatingNewContact()
{
	setCaption( "Create new contact -- Anca" );
	okButton->setText("C&reate");

	urlEdit->setReadOnly(false);
	nickEdit->setReadOnly(false);
	firstNameEdit->setReadOnly(false);
	lastNameEdit->setReadOnly(false);
	mailEdit->setReadOnly(false);
	phoneEdit->setReadOnly(false);
	mobileEdit->setReadOnly(false);
	streetEdit->setReadOnly(false);
	cityEdit->setReadOnly(false);
	codeEdit->setReadOnly(false);
	stateEdit->setReadOnly(false);
	orgEdit->setReadOnly(false);
	posEdit->setReadOnly(false);
	typeEdit->setReadOnly(false);
	roomEdit->setReadOnly(false);
}
