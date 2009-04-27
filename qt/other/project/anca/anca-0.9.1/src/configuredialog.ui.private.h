#include <qmessagebox.h>

/***************** ConfigureAction ************************/

class ConfigureActionList;


/**
 * @short This class represents a change in a configuration.
 */
class ConfigureAction
{
friend class ConfigureActionList;
public:
	/**
	 * Constructor.
	 * @param configKey key of config item with which the ConfigureAction is associated
	 */
	ConfigureAction( const QString& configKey ): m_key(configKey), m_notifySameMore(false) {}
	/**
	 * Destructor.
	 */
	~ConfigureAction() {}
	
	/**
	 * Set the new value of the config item.
	 * @param value new value
	 * @param notifySameMore see AncaConf::writeEntry(). Once it is set, it cannot be unset.
	 */
	void set( const QString& value, bool notifySameMore = false )
	{
		m_value = value;
		if( notifySameMore ) m_notifySameMore = true;
	}
	
	/**
	 * Apply made changes. Calls AncaConf::writeEntry();
	 */
	void apply()
	{
		ancaConf->writeEntry( m_key, m_value, m_notifySameMore );
	}

protected:
	QString m_key;
	QString m_value;
	bool m_notifySameMore;
};

class ConfigureActionList: public QPtrList<ConfigureAction>
{
protected:
	int compareItems( QPtrCollection::Item item1, QPtrCollection::Item item2 )
	{
		ConfigureAction *a1 = (ConfigureAction*)item1;
		ConfigureAction *a2 = (ConfigureAction*)item2;
		return a1->m_key == a2->m_key ? 0 : 1;
	}
};


/***************** PluginsViewItem ************************/

class PluginsViewItem: public QCheckListItem
{
public:
	PluginsViewItem( ConfigureDialog *dlg, QListView *parent, const PluginInfoPair& pair )
	: QCheckListItem(parent, QString::null, QCheckListItem::CheckBox), m_pair(pair), dlg(dlg)
	{
#ifndef Q_WS_QWS
		setTristate(true);
#endif
		init();
	}
	
	inline PluginInfo *info() { return m_pair.first; }

	void init()
	{
		if( !info() ) return;
	
		setText(1, info()->name );
	
		if( pluginFactory->loaded(m_pair) )
#ifndef Q_WS_QWS
			setState( QCheckListItem::On );
#else
                        setOn(true);
#endif 
		else
#ifndef Q_WS_QWS 
			setState( QCheckListItem::Off );
#else
                        setOn(false);
#endif 
	}
	
	bool loaded() { return info() && info()->get(); }
	
	bool hasConfig() { return info() && (info()->flags & P_HAS_CONFIG ); }

	bool canUnload() { return info() && (info()->flags & P_CAN_UNLOAD ); }
	
	QString description()
	{
		if( !info() ) return "No description available";
		return info()->description;
	}
	
	bool isType( Plugin::Type type ) { return info() && (info()->type == type); }
	
	PluginInfoPair& pair() { return m_pair; }

protected:
	void stateChange( bool )
	{
		// CheckBox toggles in the order: Off -> NoChange -> On

		bool loaded = pluginFactory->loaded(m_pair);

#ifndef Q_WS_QWS
		QCheckListItem::ToggleState s = state();

		//manage CheckBox
		if( s == QCheckListItem::NoChange && !loaded ) {

			setState(QCheckListItem::On);
                        return;
                }
		else if( s == QCheckListItem::Off && loaded && !canUnload() ) {
			setState(QCheckListItem::NoChange);
			return;
		}

		//we don't need to load/unload plugin
		if( (loaded && state() == QCheckListItem::On) || (!loaded && state() != QCheckListItem::On) ) return;

		if( state() == QCheckListItem::On ) {
#else
		//we don't need to load/unload plugin
		if( (loaded && isOn()) || (!loaded && !isOn()) || (loaded && !canUnload()) ) return;

		if( isOn() ) {
#endif
			pluginFactory->load( m_pair );

			//if newly loaded plugin is VideoIn, register its capabilities...
			if( isType( Plugin::VideoIn ) ) {
				VideoInPlugin *videoPlugin = (VideoInPlugin*)pluginFactory->getPlugin(Plugin::VideoIn);
				if( videoPlugin ) {
					videoPlugin->addCapabilities();
					dlg->insertVideoConfig();
				}
			}
			else if( isType( Plugin::Audio ) ) {
				AudioPlugin *audioPlugin = (AudioPlugin*)pluginFactory->getPlugin(Plugin::Audio);
				if( audioPlugin ) {
					audioPlugin->addCapabilities();
					dlg->insertAudioConfig();
				}
			}
		}
		else {
			if( m_pair.first->type == Plugin::Audio ) {
				QMessageBox mb( "Unload plugin -- Anca",
						"You are going to unload audio plugin. Since then you will not be able to communicate.\n"
								"Do you really want to unload audio plugin?",
						QMessageBox::Information,
						QMessageBox::Yes | QMessageBox::Default,
						QMessageBox::Cancel | QMessageBox::Escape,
						QMessageBox::NoButton );
				mb.setButtonText( QMessageBox::Yes, "&Unload" );
				if( mb.exec() != QMessageBox::Yes ) {
					setOn(true);
					return;
				}
			}
			pluginFactory->unload( m_pair );
		}
	}

	PluginInfoPair m_pair;

private:
	ConfigureDialog *dlg;
};


/***************** ConfigureDialogPrivate *****************/

class ConfigureDialogPrivate
{
public:
	ConfigureDialogPrivate()
	{
		configList.setAutoDelete(true);
	}

	ConfigureActionList configList;
	typedef QMap<QString, ConfigureAction*> ConfigKeyMap;
	ConfigKeyMap configKeyMap;
	QStringList existingAttrs;
	bool personalChanged;
	bool disableChanges;
};
