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

#ifndef ADDRESSBOOKPLUGIN_H
#define ADDRESSBOOKPLUGIN_H

#include "plugin.h"

class MyPlugin: public Plugin
{
public:
	MyPlugin( PluginInfo *info );

	void init( PConfigArgs& );
	void exit();

	QWidget *createGUI();
	void setPalette( const QPalette& pal );

	QString icon() { return "addressbook.png"; }
	QString text() { return "Addressbook"; }
	Plugin::Significance significance() { return Plugin::High; }
	Plugin::GUIType guiType() { return Plugin::GUIAlways; }
};

#endif

