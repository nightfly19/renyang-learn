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

#include <qobject.h>
#include "plugin.h"

class HTMLDialog;

class MyPlugin: public QObject, public Plugin
{
	Q_OBJECT
public:
	MyPlugin( PluginInfo *info );

	void init( PConfigArgs& );
	void exit();

	QWidget *createGUI();
	void setPalette( const QPalette& pal );

	QString icon() { return "htmlbrowser.png"; }
	QString text() { return "Browser"; }
	Plugin::Significance significance() { return Plugin::Medium; }
	Plugin::GUIType guiType() { return Plugin::GUIAlways; }

protected slots:
	void httpPush( const QVariant& url );
	void httpDelete( const QVariant& url );

private:
	HTMLDialog *dlg;
};

#endif

