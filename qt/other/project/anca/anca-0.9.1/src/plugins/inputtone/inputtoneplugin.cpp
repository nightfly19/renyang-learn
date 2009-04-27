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

#include "inputtoneplugin.h"
#include "dialwidget.h"

#include <qapplication.h>
#include <qpushbutton.h>

#include <ptlib.h>
#include <h323.h>

InputTonePlugin *plugin = 0;
DialWidget *widget = 0;

InputTonePlugin::InputTonePlugin( PluginInfo *info ): Plugin(info)
{
	plugin = this;
}

void InputTonePlugin::init( PConfigArgs& )
{
}

void InputTonePlugin::exit()
{
	delete widget; 

	plugin = 0;
}

QWidget *InputTonePlugin::createGUI()
{
	widget = new DialWidget( 0, "DialWidget" );
	connect( widget, SIGNAL( sendTone(char) ), this, SLOT( sendTone(char) ) );

	return widget;
}

void InputTonePlugin::setPalette( const QPalette& pal )
{
	if( widget ) {
		widget->setPalette(pal);
		return;

		QPalette p = QApplication::palette();
		widget->button1->setPalette(p);
		widget->button2->setPalette(p);
		widget->button3->setPalette(p);
		widget->button4->setPalette(p);
		widget->button5->setPalette(p);
		widget->button6->setPalette(p);
		widget->button7->setPalette(p);
		widget->button8->setPalette(p);
		widget->button9->setPalette(p);
		widget->button0->setPalette(p);
		widget->buttonx->setPalette(p);
		widget->buttono->setPalette(p);
	}
}

void InputTonePlugin::sendTone( char tone )
{
	if( !endPoint ) return;
	
	H323Connection *c = endPoint->FindConnectionWithLock( currentCallToken.latin1() );
	if( !c ) return;

	c->SendUserInputTone(tone);
	c->Unlock();
}

/***************** plugin stuff ********************/

Plugin *createInputTonePlugin();
Plugin *getInputTonePlugin();

static PluginInfo pluginInfo = {
                                   Plugin::Other,
                                   P_HAS_GUI | P_CAN_UNLOAD,
                                   "inputtone",
                                   "Sends DTMF tones when in call.",
                                   "Tomas Potrusil",
                                   createInputTonePlugin,
                                   getInputTonePlugin,
				   6
                               };

Plugin *createInputTonePlugin()
{
	return new InputTonePlugin( &pluginInfo );
}

Plugin *getInputTonePlugin()
{
	return plugin;
}

extern "C" PluginInfo *getPluginInfo()
{
	return & pluginInfo;
}

