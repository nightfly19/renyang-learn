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

#ifndef AUDIOPLUGIN_H
#define AUDIOPLUGIN_H

#include "plugin.h"
#include "audioconfigwidget.h"

#include <ptlib.h>
#include <h323.h>

#include <qptrlist.h>
#include <qobject.h>

class MyAudioPlugin: public QObject, public AudioPlugin
{
	Q_OBJECT
public:
	MyAudioPlugin( PluginInfo *info );

	Plugin::Significance significance() { return Plugin::High; }

	void init( PConfigArgs& );

	void exit();

	ConfigWidget *createCodecsConfig( QWidget *parent );
	QDialog *createConfig( QWidget *parent );

	void addCapabilities();

	PChannel *getChannel( bool recording = true );
	void prepareForCall();
	void startPreview();
	void stopPreview();

protected:
	void setSoundDevices();
	BOOL setSoundDevice( const QString& device, PSoundChannel::Directions dir);

	void configureCapList();
	void createViewItems();

protected slots:
	void updateCapabilities( const QValueList<CapabilityItem*>& capList );
	void resetView();

	void recorderDeviceChanged();
	void playerDeviceChanged();

protected:
	PConfigArgs *args;

	AudioConfigWidget *widget;

	QValueList<CapabilityItem*> capList;
};


#endif
