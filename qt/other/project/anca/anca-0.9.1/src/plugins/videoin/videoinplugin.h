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

#ifndef VIDEOIN_H
#define VIDEOIN_H

#include "plugin.h"
#include "videograbber.h"

class MyPlugin: public VideoInPlugin
{
public:
	MyPlugin( PluginInfo *info );

	Plugin::Significance significance() { return Plugin::Medium; }

	void init( PConfigArgs& );

	void exit();

	void startPreview();
	
	//asynchronous stop
	void stopPreview();

	QWidget *createGUI();
	QDialog *createConfig( QWidget *parent );
	ConfigWidget *createCodecsConfig( QWidget *parent );
	void setPalette( const QPalette& pal );
	
	PChannel *getChannel( bool recording = true );
	
	void prepareForCall();
	
	void addCapabilities();

	QWidget *widget;

protected:
	VideoGrabber *grabber;
	ConfigWidget *config;
	PConfigArgs *args;

private:
	PStringArray codecNames;
};

#endif
