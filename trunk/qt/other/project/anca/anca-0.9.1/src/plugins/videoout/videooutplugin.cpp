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

#include "videooutplugin.h"
#include "videowidget.h"
#include "configdialog.h"

MyVideoOutPlugin *plugin = 0;

MyVideoOutPlugin::MyVideoOutPlugin( PluginInfo *info ): VideoOutPlugin(info)
{
	plugin = this;
}

PVideoOutputDevice *MyVideoOutPlugin::createVideoOutputDevice( QWidget *videoWidget )
{
	if( strcmp(videoWidget->name(), "VideoWidget") == 0 )
		return new VideoOutputDevice( (VideoWidget*)videoWidget );
	else {
		PTRACE(1, "Couldn't get information to create VideoOutputDevice\n\t\tName of videoWidget: " << videoWidget->name() );
		return 0;
	}
}

QWidget *MyVideoOutPlugin::createVideoWidget()
{
	QWidget *w = new VideoWidget(0, "VideoWidget");
	list.append(w);
	return w;
}

void MyVideoOutPlugin::deleteVideoWidget( QWidget *widget )
{
	list.remove(widget);
	delete widget;
}

void MyVideoOutPlugin::init( PConfigArgs& )
{}

void MyVideoOutPlugin::exit()
{
	Q_ASSERT( list.isEmpty() );
	plugin = 0;
}

QDialog *MyVideoOutPlugin::createConfig( QWidget *parent )
{
	return new ConfigDialog(parent);
}

/***************** plugin stuff ********************/

Plugin *createMyVideoOutPlugin();
Plugin *getMyVideoOutPlugin();

static PluginInfo pluginInfo = {
                                   Plugin::VideoOut,
                                   P_HAS_CONFIG,
                                   "videoout",
                                   "Display video stream",
                                   "Tomas Potrusil",
                                   createMyVideoOutPlugin,
                                   getMyVideoOutPlugin,
				   1
                               };

Plugin *createMyVideoOutPlugin()
{
	return new MyVideoOutPlugin( &pluginInfo );
}

Plugin *getMyVideoOutPlugin()
{
	return plugin;
}

extern "C" PluginInfo *getPluginInfo()
{
	return & pluginInfo;
}
