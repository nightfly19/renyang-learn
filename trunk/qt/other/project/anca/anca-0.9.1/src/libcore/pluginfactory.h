//
// C++ Interface: pluginfactory
//
// Description:
//
//
// Author: Tom Potrusil <tom@matfyz.cz>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PLUGINFACTORY_H
#define PLUGINFACTORY_H

#include "plugin.h"

#include <qobject.h>
#include <qmap.h>
#include <qvaluelist.h>
#include <qstringlist.h>
#include <qpair.h>
#include <qlibrary.h>

typedef QPair<PluginInfo*, QLibrary*>  PluginInfoPair;
class PluginInfoList: public QValueList<PluginInfoPair>
{
public:
	bool contains( const PluginInfoPair& pair )
	{
		PluginInfo *info1 = pair.first;
		PluginInfo *info2 = 0;

		if( !info1 ) return false;

		for( Iterator it = begin(); it != end(); ++it ) {
			info2 = (*it).first;
			if( info2 && info2->id == info1->id ) return true;
		}
		return false;
	}
};

typedef QPtrList<Plugin> PluginList;
typedef QMap<Plugin::Type, PluginList> PluginListMap;

class PluginFactoryPrivate;

class PluginFactory: public QObject
{
	Q_OBJECT
public:
	PluginFactory( PConfigArgs& );
	virtual ~PluginFactory();
	

	/**
	 * Get list of all loaded plugins of the type 'type'
	 */
	PluginList *getPluginList( Plugin::Type type );
	/**
	 * Get the first loaded plugin of the type 'type'
	 */
	Plugin *getPlugin( Plugin::Type type );

	PluginInfoList& rescan();
	PluginInfoList& plugins();

	void load( const PluginInfoPair& pair );
	void unload( const PluginInfoPair& pair );

	/**
	 * @returns true if plugin is loaded
	 */
	bool loaded( const PluginInfoPair& pair );

	/**
	 * Enumerates actions that will be taken on startup (when load() is called)
	 */
	enum OnStartupAction { Invalid, Load, NotLoad };
	/**
	 * @returns action which will be taken on startup
	 */
	OnStartupAction onStartup( const PluginInfoPair& pair );

	/**
	 * Load all required plugins
	 */
	void load();

	void createGUIs();

	void setGUIPluginsPalette( const QPalette& pal );

signals:
	void insertWidget( QWidget *widget, Plugin::Type, const QString& text, const QString& icon, Plugin::Significance, Plugin::GUIType );
	void removeWidget( QWidget *widget );

protected:
	bool noConflicts( Plugin::Type );

	Plugin *createPlugin( const PluginInfoPair& pair );
	void destroyPlugin( const PluginInfoPair& pair );

private:
	PConfigArgs& args;

	/**
	 * list of all loaded plugins
	 */
	PluginListMap pluginMap;

	/**
	 * list of all known plugins
	 */
	PluginInfoList infoList;

	PluginFactoryPrivate *d;

	typedef QMap<int, int> OnStartupMap;
	OnStartupMap onStartupMap;

	typedef QMap<int, QWidget*> InsertedWidgets;
	/**
	 * map of all inserted widget: ID -> QWidget
	 */
	InsertedWidgets insertedWidgets;

	typedef QMap<int, Plugin*> IdMap;
	/**
	 * map of all loaded plugins: ID -> Plugin*
	 */
	IdMap idMap;
};
extern PluginFactory *pluginFactory;

#endif
