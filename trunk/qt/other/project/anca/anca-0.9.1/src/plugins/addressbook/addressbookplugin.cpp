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

#include "addressbookplugin.h"
#include "addressbookwidget.h"
#include "addressbook.h"
#include "listview.h"

#include <qwidgetstack.h>
#include <qapplication.h>
#include <qtoolbutton.h>
#include <qpushbutton.h>
#include <qlineedit.h>

MyPlugin *plugin = 0;
AddressBookWidget *widget = 0;
AddressBook *addressBook = 0;

MyPlugin::MyPlugin( PluginInfo *info ): Plugin(info)
{
	plugin = this;
}

void MyPlugin::init( PConfigArgs& )
{
	addressBook = new AddressBook();
}

void MyPlugin::exit()
{
	addressBook->save();

	delete widget; 
	delete addressBook;
	plugin = 0;
}

QWidget *MyPlugin::createGUI()
{
	widget = new AddressBookWidget( 0, "AddressbookWidget" );
	widget->setAddressBook(addressBook);
	addressBook->load();

	return widget;
}

void MyPlugin::setPalette( const QPalette& pal )
{
	if( widget ) {
		QPalette p = pal;
		QColorGroup cg = p.active();
		cg.setColor( QColorGroup::Base, pal.active().background());
		cg.setColor( QColorGroup::Text, pal.active().foreground());
		p.setActive(cg);
		p.setInactive(cg);
		p.setDisabled(cg);
		widget->setPalette(p);
		widget->searchEdit->setPalette(pal);
		return;

		widget->historyListView->setPalette(pal);
		widget->topTenListView->setPalette(pal);
		widget->allListView->setPalette(pal);
		widget->resultListView->setPalette(pal);
		return;

		//QPalette p = QApplication::palette();
		//widget->aButton->setPalette(p);
		//widget->dButton->setPalette(p);
		//widget->gButton->setPalette(p);
		//widget->jButton->setPalette(p);
		//widget->mButton->setPalette(p);
		//widget->pButton->setPalette(p);
		//widget->sButton->setPalette(p);
		//widget->vButton->setPalette(p);
		//widget->newContactButton->setPalette(p);
	}
}

/***************** plugin stuff ********************/

Plugin *createMyPlugin();
Plugin *getMyPlugin();

static PluginInfo pluginInfo = {
                                   Plugin::Other,
                                   P_HAS_GUI | P_CAN_UNLOAD,
                                   "addressbook",
                                   "Addressbook",
                                   "Tomas Potrusil",
                                   createMyPlugin,
                                   getMyPlugin,
				   3
                               };

Plugin *createMyPlugin()
{
	return new MyPlugin( &pluginInfo );
}

Plugin *getMyPlugin()
{
	return plugin;
}

extern "C" PluginInfo *getPluginInfo()
{
	return & pluginInfo;
}

