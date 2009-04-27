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

#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include <qobject.h>
#include <qlayout.h>

#include "plugin.h"

/**
 * @short This class manages all GUI operations
 *
 * It changes state of DisplayPanel, inserts widgets to it (from Plugins for
 * example), changes features of GUI when a call is established or cleared.
 */
class GUIManager: public QObject
{
	Q_OBJECT
public:
	GUIManager();
	~GUIManager();
	
	/**
	 * @short Returns video widget.
	 * @returns A widget that is responsible for video imaging. This widget
	 * is useful for a video plugin when creating PVideoOutputDevice device
	 * (see VideoOutPlugin::createVideoOutputDevice). We need this device
	 * in EndPoint::OpenVideoChannel.
	 * 
	 * This funtion is called when EndPoint::OpenVideoChannel is called for
	 * video decoding.
	 */
	QWidget *getVideoWidget();

public slots:
	/**
	 * @short Callback for call was started
	 *
	 * This slot should be called when new call was started and we want to
	 * rearrange GUI.
	 */
	void callStarted();

	/**
	 * @short Callback for call was cleared
	 *
	 * This slot should be called when a call was cleared and we want to
	 * rearrange GUI.
	 */
	void callCleared();
	
	/**
	 * @short Callback for calling was started
	 *
	 * This slot should be called when we are just calling somebody and we
	 * want to rearrange GUI.
	 */
	void callingStarted();

	/**
	 * @short Callback for call was held.
	 *
	 * This slot should be called when a call was held and we want to
	 * rearrange GUI.
	 */
	void callHeld();

	/**
	 * @short Hide preview display
	 *
	 * This is actually called when VideoGrabber was suspended (due to an error
	 * or whatever). It is only useful when the preview display was the
	 * current one. It doesn't stop VideoGrabber!
	 */
	void hidePreview();

	/**
	 * @short Insert a widget to DisplayPanel
	 * @param widget A widget to be inserted
	 * @param type Type of \p widget
	 * @param buttonText Text label of the inserted button
	 * @param buttonIcon Pixmap label of the inserted button
	 * @param significance A significance of the plugin
	 *
	 * This is called by PluginManager when plugins want to display someting.
	 * It inserts the \p widget to DisplayPanel, inserts appropriate button
	 * to AncaForm::buttonGroup, connects some signals and maybe does some
	 * other stuff.
	 */
	void insertWidget( QWidget *widget, Plugin::Type type, const QString& buttonText = QString::null, 
			const QString& buttonIcon = QString::null, Plugin::Significance significance = Plugin::Small,
			Plugin::GUIType guiType = Plugin::GUINoCall );

	/**
	 * @short Remove \p widget from DisplayPanel.
	 */
	void removeWidget( QWidget *widget );

	/**
	 * @short Change state of DisplayPanel.
	 * @param id ID of the widget that is to be shown.
	 *
	 * Display widget with ID \p id and make some connected work around it, eg.
	 * disable buttons etc.
	 */
	void displayChanged( int id );

	/**
	 * @short Show info widget in DisplayPanel.
	 */
	void displayInfo();
	
	/**
	 * @short Set color palette of whole application
	 *
	 * For now it changes just widgets in DisplayPanel, eg. GUI plugins.
	 */
	virtual void setColors();

protected:
	/**
	 * For internal purposes. Manage GUI to display widget with ID \p id.
	 * Called by displayChanged()
	 */
	void changeToID( int id );

	/**
	 * For internal purposes. Manage plugin buttons according to plugin's
	 * GUIType and inCall flag here.
	 * @param callHeld call was held -> it is almost the same as in call
	 */
	void configureButtons( bool callHeld = false );

	/** Enumerates types of plugins. */
	enum Type { 
		Preview, 
		Call, 
		Info, 
		Other 
	};
	/** Maps type of a plugin to its widget (id of widget in the DisplayPanel). */
	typedef QMap<int, Type> IdMap;
	/** Maps an id of a widget in DisplayPanel to a plugin that contains the widget. */
	typedef QMap<Type, int> TypeMap;

	IdMap idMap;
	TypeMap typeMap;

	QWidget *videoWidget;

	bool inCall;
	QMap<int, Plugin::GUIType> guiTypeMap;

private:
	int currentID;
	int lastNonCallID;
};
extern GUIManager *guiManager;

#endif
