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

#include "htmlbrowserplugin.h"
#include "htmlbrowserwidget.h"
#include "htmldialog.h"
#include "textbrowser.h"
#include "ancainfo.h"
#include "stdinfo.h"

#include <qapplication.h>
#include <qlineedit.h>

MyPlugin *plugin = 0;
HTMLBrowserWidget *widget = 0;

MyPlugin::MyPlugin( PluginInfo *info ): Plugin(info)
{
	plugin = this;
}

void MyPlugin::init( PConfigArgs& )
{
	ancaInfo->connectNotify( HTTP_PUSH, this, SLOT( httpPush( const QVariant& ) ) );
	ancaInfo->connectNotify( HTTP_CLOSE, this, SLOT( httpDelete( const QVariant& ) ) );
}

void MyPlugin::exit()
{
	ancaInfo->disconnectNotify( HTTP_PUSH, this, SLOT( httpPush( const QVariant& ) ) );
	ancaInfo->disconnectNotify( HTTP_CLOSE, this, SLOT( httpDelete( const QVariant& ) ) );
	delete widget; 
	plugin = 0;
}

QWidget *MyPlugin::createGUI()
{
	widget = new HTMLBrowserWidget( 0, "HTMLBrowserWidget" );

	return widget;
}

void MyPlugin::setPalette( const QPalette& pal )
{
	if( widget ) {
		widget->setPalette(pal);
		return;

		QPalette p = QApplication::palette();
		widget->sourceEdit->setPalette(p);
		widget->htmlBrowser->setPalette(p);
	}
}

void MyPlugin::httpPush( const QVariant& url )
{
	dlg = new HTMLDialog(widget, "htmlDialog");
	dlg->setURL(url.toString());
	dlg->exec();
	
	delete dlg;
	dlg = 0;
}

void MyPlugin::httpDelete( const QVariant& url )
{
	if( dlg )
		dlg->closeDialog( url.toString() );
}

/***************** plugin stuff ********************/

Plugin *createMyPlugin();
Plugin *getMyPlugin();

static PluginInfo pluginInfo = {
                                   Plugin::Other,
                                   P_HAS_GUI | P_CAN_UNLOAD,
                                   "htmlbrowser",
                                   "Very simple HTML browser. It allows just basic formatting. All input is ignored.",
                                   "Tomas Potrusil",
                                   createMyPlugin,
                                   getMyPlugin,
				   4
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

