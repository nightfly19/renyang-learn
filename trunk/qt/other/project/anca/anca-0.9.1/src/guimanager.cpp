/***************************************************************************
*   Copyright (C) 2004 by Tom Potrusil                                    *
*   tom@matfyz.cz                                                         *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

#include "guimanager.h"

#include "ancaform.h"
#include "pluginfactory.h"
#include "ancaconf.h"
#include "confdefines.h"
#include "ancainfo.h"
#include "stdinfo.h"

#include <ptlib.h>

#include <qlayout.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qapplication.h>
#include <qpopupmenu.h>
#include <qtoolbutton.h>
#include <qwidgetstack.h>
#include <qstatusbar.h>
#include <qaction.h>

GUIManager *guiManager = 0;

GUIManager::GUIManager() :videoWidget(0), inCall(false)
{
	PTRACE(6, "GUIManager constructor");

	guiManager = this;

	setColors();

	int id = ancaForm->displayStack->id(ancaForm->callPage);
	idMap[id] = Call;
	typeMap[Call] = id;

	id = ancaForm->displayStack->id(ancaForm->infoPage);
	idMap[id] = Info;
	typeMap[Info] = id;

	// I suppose that infoPage is the initial one
	currentID = id;
	lastNonCallID = id;

	connect( pluginFactory, SIGNAL( insertWidget( QWidget*, Plugin::Type, const QString&, const QString&, Plugin::Significance, Plugin::GUIType ) ), this, SLOT( insertWidget( QWidget*, Plugin::Type, const QString&, const QString&, Plugin::Significance, Plugin::GUIType ) ) );
	connect( pluginFactory, SIGNAL( removeWidget( QWidget* ) ), this, SLOT( removeWidget( QWidget* ) ) );
	
	connect( ancaForm->buttonGroup, SIGNAL(clicked(int)), this, SLOT(displayChanged(int)));
	connect( ancaForm->pluginsMenu, SIGNAL(activated(int)), this, SLOT(displayChanged(int)));
	connect( ancaForm->mainButton, SIGNAL(clicked()), this, SLOT(displayInfo()));

	Plugin *p = pluginFactory->getPlugin(Plugin::VideoOut);
	if( p ) {
		VideoOutPlugin *plugin = (VideoOutPlugin*)p;
		videoWidget = plugin->createVideoWidget();
		ancaForm->setVideoWidget(videoWidget);
	}
}

GUIManager::~GUIManager()
{
	guiManager = 0;

	VideoOutPlugin *plugin = (VideoOutPlugin*)pluginFactory->getPlugin(Plugin::VideoOut);
	if( plugin )
		plugin->deleteVideoWidget(videoWidget);
}

void GUIManager::setColors()
{
	QPalette pal;
	QColorGroup cg = QApplication::palette().active();
	cg.setColor( QColorGroup::Foreground, QColor( 236, 236, 236 ) );//very light gray
//	cg.setColor( QColorGroup::Button, darkGray );
	cg.setColor( QColorGroup::Light, gray );
	cg.setColor( QColorGroup::Midlight, gray );
	cg.setColor( QColorGroup::Dark, darkGray );
	cg.setColor( QColorGroup::Mid, yellow );
	cg.setColor( QColorGroup::Text, black );
	cg.setColor( QColorGroup::BrightText, QColor( 255, 255, 127 ) );//light yellow
//	cg.setColor( QColorGroup::ButtonText, lightGray );
	cg.setColor( QColorGroup::Base, white );
	cg.setColor( QColorGroup::Background, black );
	cg.setColor( QColorGroup::Shadow, black );
	cg.setColor( QColorGroup::Highlight, darkBlue );
	cg.setColor( QColorGroup::HighlightedText, QColor( 236, 236, 236 ) );
	cg.setColor( QColorGroup::Link, blue );
	cg.setColor( QColorGroup::LinkVisited, darkBlue );
	pal.setActive( cg );
	pal.setInactive( cg );
	pal.setDisabled( cg );

	//use this palette for all widgets in displayStack,
	//these are info and audio pages, and widgets from GUI plugins
	ancaForm->infoPage->setPalette(pal);
	ancaForm->callPage->setPalette(pal);
	pluginFactory->setGUIPluginsPalette(pal);
}


void GUIManager::insertWidget( QWidget *widget, Plugin::Type type, const QString& buttonText, const QString& buttonIcon, Plugin::Significance significance, Plugin::GUIType guiType )
{
	PTRACE(6, "GUIManager::insertWidget");

	QPixmap buttonPixmap;
	QString buttonString;
	
	//set up button picture
	if( !buttonIcon.isNull() )
		buttonPixmap = QPixmap::fromMimeSource( buttonIcon );
	else if( type == Plugin::VideoIn )
		buttonPixmap = QPixmap::fromMimeSource( "preview.png" );
	else
		buttonPixmap = QPixmap::fromMimeSource( "unknown.png" );

	//set up button text
	if( !buttonText.isNull() )
		buttonString = buttonText;
	else if( type == Plugin::VideoIn )
		buttonString = "Video preview";
	else
		buttonString = "Noname";

	int id = ancaForm->displayStack->addWidget(widget);
	Type t;
	switch( type ) {
		case Plugin::VideoIn: t = Preview; break;
		default: t = Other; break;
	}
	idMap[id] = t;
	typeMap[t] = id;
	guiTypeMap[id] = guiType;

	// insert button
	switch( significance ) {
		case Plugin::High:
			ancaForm->insertButton( buttonPixmap, buttonString, id, true );
			break;
		case Plugin::Medium:
			if( ancaForm->buttonGroup->count() < ancaConf->readIntEntry( MAX_DIRECTLY_VISIBLE_PLUGINS, MAX_DIRECTLY_VISIBLE_PLUGINS_DEFAULT ) ) {
				ancaForm->insertButton( buttonPixmap, buttonString, id, false );
				break;
			}
		case Plugin::Small:
			ancaForm->pluginsMenu->insertItem( QIconSet( buttonPixmap ), buttonString, id );
			if( !ancaForm->mainButton->isVisible() ) 
				ancaForm->mainButton->show();
	}

	configureButtons();
}

void GUIManager::removeWidget( QWidget *widget )
{
	displayInfo();

	int id = ancaForm->displayStack->id(widget);
	PTRACE( 10, "Removing ID " << id );

	if( ancaForm->buttonGroup->find( id ) )
		ancaForm->removeButton( id );
	else
		ancaForm->pluginsMenu->removeItem( id );
	ancaForm->displayStack->removeWidget(widget);

	//cancel mappings
	if( idMap.contains(id) ) {
		typeMap.remove(idMap[id]);
		idMap.remove(id);
		guiTypeMap.remove(id);
	}
}

void GUIManager::displayInfo()
{
	if( inCall )
		displayChanged(typeMap[Call]);
	else
		displayChanged(typeMap[Info]);
}

void GUIManager::displayChanged( int id )
{
	PTRACE(6, "GUIManager::displayChanged to " << id );

	int oldID = currentID;

	// manage widgets
	changeToID( id );
	
	// stop grabber if we just left preview window
	if( idMap[oldID] == Preview ) {
		VideoInPlugin *plugin = (VideoInPlugin*)pluginFactory->getPlugin(Plugin::VideoIn);
		if( plugin ) plugin->stopPreview();
	}
	
	// start grabber if we just opened preview window
	if( idMap[id] == Preview ) {
		VideoInPlugin *plugin = (VideoInPlugin*)pluginFactory->getPlugin(Plugin::VideoIn);
		if( plugin ) plugin->startPreview();
	}
}

void GUIManager::changeToID( int id )
{
	/////// manage buttons in buttonGroup and in popup menu

	QButton *clickedButton = ancaForm->buttonGroup->find(id);
	QButton *currentButton = currentButton = ancaForm->buttonGroup->find(currentID);

	// button in buttonGroup was on, should I switch it off?
	if( currentButton ) {
		// yes do it yourself
		if ( currentButton != clickedButton ) {
			// test if really the clicked button is on now
			if( clickedButton && !clickedButton->isOn() )
				clickedButton->toggle();
			currentButton->toggle();
		}
		// the button was clicked, display main display (info)
		// test if really the button is off now
		else if ( !currentButton->isOn() ) {
			if( inCall )
				id = typeMap[Call];
			else
				id = typeMap[Info];
		}
	}
	
	/////// manage display

	ancaForm->displayStack->raiseWidget(id);

	currentID = id;
}

void GUIManager::callStarted()
{
	PTRACE(6, "GUIManager::callStarted");

	inCall = true;

	//enable/disable plugin buttons
	configureButtons();
	ancaForm->configButton->setEnabled(false);
	ancaForm->setCallButton(true);
	ancaForm->hideCallingProgress();
	ancaForm->statusBar()->clear();
	ancaForm->HoldCallAction->setEnabled(true);
	ancaForm->HoldCallAction->setText("Hold Call");
	ancaForm->TransferAction->setEnabled(true);
	
	lastNonCallID = currentID;
	displayChanged( typeMap[Call] );
}

void GUIManager::callHeld()
{
	PTRACE(6, "GUIManager::callHeld");

	inCall = false;

	//enable/disable plugin buttons
	configureButtons(true);
	ancaForm->configButton->setEnabled(false);
	ancaForm->setCallButton(false);
	ancaForm->hideCallingProgress();
	ancaForm->statusBar()->clear();
	ancaForm->HoldCallAction->setEnabled(true);
	ancaForm->HoldCallAction->setText("Retrieve Call");
	ancaForm->TransferAction->setEnabled(false);
	
	displayChanged( lastNonCallID );
}

void GUIManager::callCleared()
{
	PTRACE(6, "GUIManager::callCleared");

	inCall = false;

	//enable/disable plugin buttons
	configureButtons();
	ancaForm->configButton->setEnabled(true);
	ancaForm->setCallButton(false);
	ancaForm->hideCallingProgress();
	ancaForm->statusBar()->clear();
	ancaForm->HoldCallAction->setEnabled(false);
	ancaForm->HoldCallAction->setText("Hold Call");
	ancaForm->TransferAction->setEnabled(true);
	
	displayChanged( lastNonCallID );
}

void GUIManager::callingStarted()
{
	inCall = false;

	ancaForm->setCallButton(true);
	ancaForm->showCallingProgress();
	ancaForm->statusBar()->message( "Calling " + ancaInfo->get(ADDRESS_CALLING));
}

QWidget *GUIManager::getVideoWidget()
{
	return videoWidget;
}

void GUIManager::hidePreview()
{
	displayChanged( typeMap[Info] );
}

void GUIManager::configureButtons( bool callHeld )
{
	QButton *button = 0;
	QMap<int,Plugin::GUIType>::Iterator it;
        for ( it = guiTypeMap.begin(); it != guiTypeMap.end(); ++it ) {
		switch( it.data() ) {
			case Plugin::GUINoCall:
				ancaForm->pluginsMenu->setItemEnabled(it.key(), !inCall && !callHeld);
				if( (button = ancaForm->buttonGroup->find(it.key())) )
					button->setEnabled(!inCall && !callHeld);
				break;
			case Plugin::GUICall:
				ancaForm->pluginsMenu->setItemEnabled(it.key(), inCall);
				if( (button = ancaForm->buttonGroup->find(it.key())) )
					button->setEnabled(inCall);
				break;
			default:
				break;
		}
        }
}

