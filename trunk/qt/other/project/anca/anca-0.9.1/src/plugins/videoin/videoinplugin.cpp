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

#include "videoinplugin.h"

#include "pluginfactory.h"
#include "ancaconf.h"
#include "configdialog.h"

#include <qapplication.h>

#include <ptlib.h>
#include <h323.h>

MyPlugin *plugin = 0;
PVideoOutputDevice *createVideoOutputDevice();

MyPlugin::MyPlugin( PluginInfo *info ): VideoInPlugin(info), widget(0), grabber(0), config(0)
{
	plugin = this;
}

void MyPlugin::init( PConfigArgs& args_)
{
	args = &args_;
	grabber = new VideoGrabber(args);
}

void MyPlugin::exit()
{
	grabber->close();
	grabber->WaitForTermination();
	delete grabber;

	if( endPoint )
		endPoint->RemoveCapabilities( codecNames );
	if( config )
		delete config;

	VideoOutPlugin *videoOutPlugin = (VideoOutPlugin *)pluginFactory->getPlugin( Plugin::VideoOut );
	if( videoOutPlugin && widget )
		videoOutPlugin->deleteVideoWidget(widget);

	plugin = 0;
}

//asynchronous start
void MyPlugin::startPreview()
{
	grabber->startPreview();
}

//asynchronous stop
void MyPlugin::stopPreview()
{
	grabber->stopPreview();
}

QWidget *MyPlugin::createGUI()
{
	if( args->HasOption("no-videolocal") ) return 0;

	VideoOutPlugin *videoOutPlugin = (VideoOutPlugin *)pluginFactory->getPlugin( Plugin::VideoOut );
	
	if ( !videoOutPlugin ) {
		PError << "You don't have video out plugin, so there will be no video preview" << endl;
		return 0;
	}

	widget = videoOutPlugin->createVideoWidget();

	grabber->registerVideoOutputDeviceCreate(&createVideoOutputDevice);

	return widget;
}

void MyPlugin::setPalette( const QPalette& pal )
{
	if( widget ) widget->setPalette(pal);
}

ConfigWidget *MyPlugin::createCodecsConfig( QWidget * /*parent*/ )
{
	config = 0;
	return config;
}

QDialog *MyPlugin::createConfig( QWidget *parent )
{
	return new ConfigDialog( parent );
}

PChannel *MyPlugin::getChannel( bool recording )
{
	if( recording )
		return grabber->getVideoChannel();

	return new PVideoChannel();
}

void MyPlugin::prepareForCall()
{
	grabber->startCall();

	while( grabber->preparing ) {
		PThread::Current()->Sleep(50);
		qApp->processEvents();
	}
}

void MyPlugin::addCapabilities()
{
	PAssertNULL( endPoint );

	VideoGrabber::VideoSize videoSize = (VideoGrabber::VideoSize)ancaConf->readIntEntry( "videoin", "videoSize", VideoGrabber::V_LARGE );

	H323_H261Capability *cap = 0;

	if (videoSize == VideoGrabber::V_LARGE) {
		/* CIF Capability in first position */
		endPoint->SetCapability (0, 1, cap = new H323_H261Capability (0, 2, FALSE, FALSE, 6217));
		codecNames.AppendString( cap->GetFormatName() );
		endPoint->SetCapability (0, 1, cap = new H323_H261Capability (4, 0, FALSE, FALSE, 6217));
		codecNames.AppendString( cap->GetFormatName() );
	}
	else {
		endPoint->SetCapability (0, 1, cap = new H323_H261Capability (4, 0, FALSE, FALSE, 6217)); 
		codecNames.AppendString( cap->GetFormatName() );
		endPoint->SetCapability (0, 1, cap = new H323_H261Capability (0, 2, FALSE, FALSE, 6217));
		codecNames.AppendString( cap->GetFormatName() );
	}
}

/***************** plugin stuff ********************/

Plugin *createMyPlugin();
Plugin *getMyPlugin();

static PluginInfo pluginInfo = {
                                   Plugin::VideoIn,
                                   P_HAS_GUI | P_HAS_CONFIG | P_CAN_UNLOAD,
                                   "videoin",
                                   "Grab video stream from v4l device",
                                   "Tomas Potrusil",
                                   createMyPlugin,
                                   getMyPlugin,
				   2
                               };

PVideoOutputDevice *createVideoOutputDevice()
{
	if( !plugin ) return 0;
	
	VideoOutPlugin *videoOutPlugin = (VideoOutPlugin *)pluginFactory->getPlugin( Plugin::VideoOut );
	if( !videoOutPlugin ) return 0;
	
	return videoOutPlugin->createVideoOutputDevice(plugin->widget);
}

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

