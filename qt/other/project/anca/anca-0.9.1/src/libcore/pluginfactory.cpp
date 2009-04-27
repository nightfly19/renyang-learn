//
// C++ Implementation: pluginfactory
//
// Description:
//
//
// Author: Tom Potrusil <tom@matfyz.cz>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "pluginfactory.h"

#include "ancaconf.h"
#include "stdconf.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <ptlib.h>

#include <qlibrary.h>
#include <qregexp.h>
#include <qdir.h>
#include <qapplication.h>

bool operator< ( PluginInfoPair& p1, PluginInfoPair& p2 ) 
{
	return p1.first->type < p2.first->type;
}

class MyPlugin
{
public:
	MyPlugin() : m_plugin( 0 ) {}
	MyPlugin( const MyPlugin& plugin ) : m_plugin( plugin.m_plugin ) {}
	MyPlugin( Plugin *plugin ): m_plugin(plugin) {}
	
	Plugin *plugin() { return m_plugin; }

	MyPlugin& operator=( const MyPlugin& plugin ) {  m_plugin = plugin.m_plugin; return *this; }

protected:
	Plugin *m_plugin;
};

bool operator< ( MyPlugin& p1, MyPlugin& p2 ) {
	// Significance = { High, Medium, Small }
	return p1.plugin()->significance() < p2.plugin()->significance();
}

class PluginFactoryPrivate
{
public:
	PluginFactoryPrivate(): paletteSet(false), forceUnload(false) {}

	typedef QValueList<MyPlugin> GuiPluginList;
	/**
	 * list of plugins that provide GUI, just for PluginFactory::createGUIs() call
	 */
	GuiPluginList guiPlugins;

	QPalette currentPalette;
	bool paletteSet;
	bool forceUnload;
};

//////////////////////////////////////////

PluginFactory *pluginFactory = 0;

PluginFactory::PluginFactory( PConfigArgs& args ): args(args), d(new PluginFactoryPrivate)
{
	pluginFactory = this;

	QValueList<int> keys = ancaConf->readIntListEntry( "onStartupMapKeys" );
	QValueList<int> values = ancaConf->readIntListEntry( "onStartupMapValues" );
	if( !keys.isEmpty() && keys.count() == values.count() ) {
		QValueList<int>::iterator it1 = keys.begin();
		QValueList<int>::iterator it2 = values.begin();
		for( ; it1 != keys.end() /* && it2 != values.end() */; ++it1, ++it2 )
			onStartupMap[*it1] = *it2;
	}
}

PluginFactory::~PluginFactory()
{
	PTRACE(6, "PluginFactory destructor");

	ancaConf->writeIntListEntry( "onStartupMapKeys", onStartupMap.keys() );
	ancaConf->writeIntListEntry( "onStartupMapValues", onStartupMap.values() );

	d->forceUnload = true;
	PluginInfoList::iterator it = infoList.end();
	do {
		--it;

		if ( it == infoList.end() ) break;

		unload(*it);
	}
	while ( it != infoList.begin() );
//	for( PluginInfoList::iterator it = infoList.begin(); it != infoList.end(); ++it ) {
//		unload(*it);
//		QLibrary *lib = (*it).second;
//		delete lib;
//	}

	delete d;
}

PluginList *PluginFactory::getPluginList( Plugin::Type type )
{
	if ( !pluginMap.contains( type ) ) {
		PTRACE(1, "PluginList of type " << type << " doesn't exist");
		return 0;
	}

	return &pluginMap[ type ];
}

Plugin *PluginFactory::getPlugin( Plugin::Type type )
{
	PluginList * list = getPluginList( type );
	if ( !list ) return 0;

	return list->first();
}

PluginInfoList& PluginFactory::rescan()
{
	//remove plugins that are not loaded

	PluginInfoList::Iterator it = infoList.begin();
	while( it != infoList.end() ) {
		PluginInfo *info = (*it).first;
		if( !info ) continue;

		//plugin is not loaded
		if( !info->get() ) {
			QLibrary *lib = (*it).second;
			lib->unload();
			delete lib;
			it = infoList.remove(it);
			continue;
		}
		++it;
	}

	//load all plugins (and avoid collisions)

	QRegExp libRxp = QRegExp( "^lib.*\\.so[0-9.]*$" );
	//QRegExp libRxp = QRegExp( "^lib.*" );

	PluginInfoList pluginList;

	QStringList libraryPaths = ancaConf->readListEntry( LIBRARY_PATHS, LIBRARY_PATHS_DEFAULT );
#ifdef PREL
	libraryPaths.append( PREL "/share/anca/plugins");
#endif
	libraryPaths.append(QDir::homeDirPath() + "/.anca/plugins");

	//iterate all paths where plugins should be
	for ( QStringList::iterator it = libraryPaths.begin(); it != libraryPaths.end(); ++it ) {
		PTRACE( 3, "Searching directory " << ( *it ).latin1() << " for plugins" );
		QDir dir( *it );
		if ( !dir.exists() ) continue;

		dir.setFilter( QDir::Files | QDir::NoSymLinks );

		const QFileInfoList *fileList = dir.entryInfoList();
		QFileInfo *file;
		QLibrary *lib;
		//iterate all files in the directory
		for ( QFileInfoListIterator fit( *fileList ); ( file = *fit ); ++fit ) {
			//is the file plugin?
			if ( !file->fileName().contains( libRxp ) ) continue;

			//open plugin
			lib = new QLibrary( file->filePath().latin1() );
			
			if( !lib->load() ) {
				PTRACE( 3, "Library " << file->fileName().latin1() << " could not be loaded" );
				continue;
			}

			//resolve symbol 'getPluginInfo'
			if ( GetPluginInfo * getPluginInfo = ( GetPluginInfo* ) lib->resolve( "getPluginInfo" ) ) {
				PluginInfo * pluginInfo = getPluginInfo();
				
				pluginList.append( PluginInfoPair(pluginInfo, lib) );
			} else {
				PTRACE( 3, "Symbol \"getPluginInfo\" not found in library " << file->fileName().latin1() );
				delete lib;
			}
		}
	}

	//insert that pluginInfo to class infoList that in not already there
	for( PluginInfoList::iterator it = pluginList.begin(); it != pluginList.end(); ++it ) {
		QLibrary *lib = (*it).second;
		if( !lib ) continue;

		if( !infoList.contains( *it ) )
			infoList.append( *it );
		else {
			lib->unload();
			delete lib;
		}
	}

	// sort plugins according to type
	qHeapSort(infoList);

	return infoList;
}

PluginInfoList& PluginFactory::plugins()
{
	return infoList;
}

void PluginFactory::load()
{
	rescan();
	
	for( PluginInfoList::iterator it = infoList.begin(); it != infoList.end(); ++it ) {
		PluginInfo *pluginInfo = (*it).first;
		QLibrary *lib = (*it).second;
		
		if ( !pluginInfo || !lib ) continue;
		
		PTRACE( 1, "\n '" << pluginInfo->name << "':\n" << 
		           "\tdescription:\t" << pluginInfo->description << "\n" << 
		           "\tauthor:\t\t" << pluginInfo->author );

		Plugin::Type type = pluginInfo->type;
		int id = pluginInfo->id;
		Plugin *plugin = 0;

		if( onStartupMap.contains(id) ) {
			if( onStartupMap[id] == NotLoad ) continue;
			plugin = createPlugin( *it );
		}
		else {
			if( ancaConf->readBoolEntry( LOAD_NEW_PLUGINS, LOAD_NEW_PLUGINS_DEFAULT ) && noConflicts(type) ) {
				onStartupMap[id] = Load;
				plugin = createPlugin( *it );
			}
			else
				onStartupMap[id] = NotLoad;
		}

		// create GUI that plugin exports?
		if( pluginInfo->flags & P_HAS_GUI ) d->guiPlugins.append(plugin);
	}
}

Plugin *PluginFactory::createPlugin( const PluginInfoPair& pair )
{
	PluginInfo *pluginInfo = pair.first;

	//create plugin
	PTRACE( 6, "Creating plugin " << pluginInfo->name );
	Plugin *plugin = pluginInfo->create();

	if ( !plugin ) {
		PTRACE( 1, "Creation of plugin " << pluginInfo->name << " failed" );
		return 0;
	}

	//insert plugin to appropriate list according to its type
	Plugin::Type type = pluginInfo->type;
	PluginList& list = pluginMap[ type ];
	list.append( plugin );
	idMap[pluginInfo->id] = plugin;

	PTRACE( 6, "Initializing plugin..." );
	plugin->init(args);

	return plugin;
}

void PluginFactory::destroyPlugin( const PluginInfoPair& pair )
{
	PluginInfo *pluginInfo = pair.first;
	Plugin *plugin = pluginInfo->get();
	if( !plugin ) return;

	PTRACE( 6, "Exiting plugin... " << pluginInfo->name );
	plugin->exit();

	//remove plugin from the lists
	Plugin::Type type = pluginInfo->type;
	PluginList& list = pluginMap[ type ];
	list.removeRef( plugin );
	idMap.remove(pluginInfo->id);

	PTRACE( 6, "Destroying plugin" );
	delete plugin;
}

void PluginFactory::createGUIs()
{
	//sort GUI plugins according to significance
	qHeapSort(d->guiPlugins);

	for( PluginFactoryPrivate::GuiPluginList::iterator it = d->guiPlugins.begin(); it != d->guiPlugins.end(); ++it ) {
		Plugin *plugin = (*it).plugin();
		if( !plugin ) continue;
		
		PTRACE( 1, "Creating GUI of plugin " << plugin->name() );
		QWidget *widget = plugin->createGUI();
		if ( widget ) {
			emit insertWidget( widget, plugin->type(), plugin->text(), plugin->icon(), plugin->significance(), plugin->guiType() );
			insertedWidgets[plugin->pluginInfo()->id] = widget;
		} else
			PTRACE( 1, "Failed" );
	}
}

void PluginFactory::setGUIPluginsPalette( const QPalette& pal )
{
	QValueList<int> ids = insertedWidgets.keys();
	for( QValueList<int>::iterator it = ids.begin(); it != ids.end(); ++it ) {
		Plugin *plugin = idMap[*it];
		if( plugin ) {
			plugin->setPalette(pal);
		}
	}

	d->currentPalette = pal;
	d->paletteSet = true;
}

bool PluginFactory::noConflicts( Plugin::Type type )
{
	if( type != Plugin::Other && getPlugin(type) ) return false;

	return true;
}

void PluginFactory::load( const PluginInfoPair& pair )
{
	PTRACE( 6, "PluginFactory::load" );
	PluginInfo *pluginInfo = pair.first;
	if( !pluginInfo ) return;

	int id = pluginInfo->id;
	onStartupMap[id] = Load;

	if( loaded(pair) ) return;

	Plugin *plugin = createPlugin( pair );

	// create GUI that plugin exports?
	if( pluginInfo->flags & P_HAS_GUI ) {
		PTRACE( 1, "Creating GUI of plugin " << plugin->name() );
		QWidget *widget = plugin->createGUI();
		if ( widget ) {
			emit insertWidget( widget, plugin->type(), plugin->text(), plugin->icon(), plugin->significance(), plugin->guiType() );
			insertedWidgets[id] = widget;
			if( d->paletteSet ) setGUIPluginsPalette(d->currentPalette);
		} else
			PTRACE( 1, "Failed" );
	}
}

void PluginFactory::unload( const PluginInfoPair& pair )
{
	PTRACE( 6, "PluginFactory::unload" );
	PluginInfo *pluginInfo = pair.first;
	if( !pluginInfo ) return;

	int id = pluginInfo->id;
	onStartupMap[id] = NotLoad;

	if( !loaded(pair) ) return;

	//do not unload the plugin if P_CAN_UNLOAD flag is not set and
	//forceUnload is false
	if( !(pluginInfo->flags & P_CAN_UNLOAD) && !d->forceUnload ) {
		PTRACE(3, "Plugin " << pluginInfo->name << " can't be unloaded.");
		return;
	}

	PTRACE(6, "Unloading plugin " << pluginInfo->name );

	if( pluginInfo->flags & P_HAS_GUI ) {
		emit removeWidget( insertedWidgets[id] );
		insertedWidgets.remove(id);
	}

	destroyPlugin(pair);
}

bool PluginFactory::loaded( const PluginInfoPair& pair )
{
	PluginInfo *pluginInfo = pair.first;
	if( !pluginInfo ) return false;

	return pluginInfo->get();
}

PluginFactory::OnStartupAction PluginFactory::onStartup( const PluginInfoPair& pair )
{
	PluginInfo *pluginInfo = pair.first;
	if( !pluginInfo ) return Invalid;

	int id = pluginInfo->id;
	return (PluginFactory::OnStartupAction)onStartupMap[id];
}
