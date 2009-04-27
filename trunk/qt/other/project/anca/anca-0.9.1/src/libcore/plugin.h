/**
 * @file plugin.h
 * This is a file containing all stuff regarding plugins.
 */

/**
 * @page plugin_page ANCA plugins
 * This page describes ANCA plugins. If you want to learn about how
 * to write them, see \ref write_plugin_page page.
 *
 * A plugin is a library that is explicitly loaded at a runtime of
 * an application, a module. It contains some external features that
 * are not required by the the core of the application, we can say
 * additions.
 *
 * ANCA plugins can extend the application in several ways. They
 * can run independently, the core knows nothing about them (except
 * some basic - control - features). These plugins can have a graphics
 * user interface (GUI) which will be accessible from the main window
 * of the application.
 *
 * Other ANCA plugins have a special type and purpose. These plugins
 * provides some features that are desirable by the application.
 * See Plugin documentation to know more about them.
 *
 * All ANCA plugins can have their configuration dialog. They can
 * be loaded and unloaded any time the application run.
 */

/**
 * @page write_plugin_page How to write plugins
 * This page describes how to write plugins for ANCA application.
 *
 * @section prereq_plugin_sec Prerequisites
 * The plugin code can should include just the libcore files from
 * the ANCA application. The libcore part is a layer between plugins
 * and the rest of the application. It contains some tools that
 * provide communication within the application (AncaConf and AncaInfo).
 * See \ref libcore_page for more information.
 *
 * @section basic_plugin_sec The basic class
 * The basic of each plugin is a class that is derived from the Plugin
 * class. In fact, Plugin is an interface (virtual class) since it
 * contains some null methods. See the documentation of it to learn
 * what does it contain and what should (and must) be implemented
 * in your plugin.
 *
 * The most important methods are the null ones: Plugin::init() and
 * Plugin::exit(). They control the state of the plugin.
 * Plugin::init() is called on the start of the plugin and Plugin::exit()
 * on the destruction of the plugin.
 *
 * There are some other special predefined plugin types:
 * - MediaPlugin
 * - AudioPlugin
 * - VideoInPlugin
 * - VideoOutPlugin
 *
 * These plugins are known to the core application and each has its
 * specific meaning.
 *
 * @section other_plugin_sec The other required stuff
 * The plugin is not just a class derived from the Plugin class. It is
 * a module that consist of some other code.
 *
 * @subsection plugininfo_subsec PluginInfo structure
 * PluginInfo is a required structure. It contains plugin's type,
 * name, description, author, unique id, information flags and
 * functions for creation and getting the basic Plugin class.
 *
 * Example:
 * @code
 * Plugin *createMyPlugin();
 * Plugin *getMyPlugin();
 * 
 * static PluginInfo pluginInfo = {
 *     Plugin::Audio,
 *     P_CAN_UNLOAD | P_HAS_CONFIG,
 *     "audio",
 *     "Configures audio codecs",
 *     "author",
 *     createMyPlugin,
 *     getMyPlugin,
 *     12345
 * };
 * 
 * Plugin *createMyPlugin()
 * {
 *     return new MyPlugin( &pluginInfo );
 * }
 * 
 * Plugin *getMyPlugin()
 * {
 *     return plugin;
 * }
 * @endcode
 *
 * @subsection getplugin_subsec getPluginInfo function
 * The PluginInfo structure must be accessible in some way. This
 * is done by the getPluginInfo() function that is the another
 * required code in your plugin.
 *
 * Example:
 * @code
 * extern "C" PluginInfo *getPluginInfo()
 * {
 *     return & pluginInfo;
 * }
 * @endcode
 */

#ifndef PLUGIN_H
#define PLUGIN_H

#include <qstring.h>
#include <qwidget.h>
#include <qlibrary.h>

class H323EndPoint;
class PChannel;
class PVideoOutputDevice;
class PluginInfo;
class PConfigArgs;

/**
 * @short This is the basic class of all plugins for ANCA application.
 *
 * See \ref plugin_page to learn more about plugins.
 * Each plugin must be a descendant of the #Plugin class.
 * It provides some information about the plugin as well as
 * some control of the plugin.
 *
 * "Static" information methods are: name(), description(),
 * author() and type(). These methods extract the information
 * from PluginInfo struture that is passed in constructor.
 * The whole PluginInfo structure can be accessed by
 * pluginInfo().
 *
 * Other information methods are: icon(), text() and
 * significance(). These can be predefined since they are
 * virtual.
 *
 * Control methods are init(), exit(), createGUI(),
 * createConfig() and setPalette().
 * 
 * init() and exit() must be defined each time since they are null.
 * They control the state of the plugin. init() is called on the
 * initialization of the plugin, exit() on the desctruction of the
 * plugin.
 *
 * createGUI() and setPalette() is called only when P_HAS_GUI flag
 * is set in the appropriate PluginInfo structure.
 * createConfig() is called only when P_HAS_CONFIG is set.
 */
class Plugin
{
friend class PluginFactory;
public:
	/** Defines the type of the plugin. */
	enum Type {
		VideoOut, /**< Plugin is derived from VideoOutPlugin. */
		VideoIn,  /**< Plugin is derived from VideoInPlugin. */
		Audio,    /**< Plugin is derived from AudioPlugin. */
		Other     /**< General plugin. */
	};

	/**
	 * Contructor.
	 * @param info pointer to the PluginInfo structure of the plugin
	 */
	Plugin( PluginInfo *info = 0 );

	virtual ~Plugin();

	/** @return name of the plugin (extracted from the relevant PluginInfo) */
	const char *name() const;
	/** @return description of the plugin (extracted from the relevant PluginInfo) */
	const char *description() const;
	/** @return author of the plugin (extracted from the relevant PluginInfo) */
	const char *author() const;
	/** @return type of the plugin (extracted from the relevant PluginInfo) */
	Type type() const;

	/**
	 * Called on the initialization of the plugin.
	 * @param arg command line parameters
	 */
	virtual void init( PConfigArgs& arg ) = 0;
	/**
	 * Called on the desctruction of the plugin.
	 */
	virtual void exit() = 0;
	
	/**
	 * Called when creating GUI of the plugin. This is done only when
	 * P_HAS_GUI flag is set in PluginInfo structure.
	 * @return widget that represents the graphics user interface of the plugin
	 */
	virtual QWidget *createGUI() { return 0; }
	/**
	 * Called when application want to open configuration
	 * dialog of the plugin. This is done only when
	 * P_HAS_CONFIG flag is set in PluginInfo structure.
	 * @param parent parent widget of the dialog
	 * @return configuration dialog of the plugin
	 */
	virtual QDialog *createConfig( QWidget *parent ) { return 0; }
	/**
	 * Set the color palette of the GUI of the plugin. This is called
	 * only when P_HAS_CONFIG flag is set in PluginInfo structure.
	 * @param p new color palette
	 */
	virtual void setPalette( const QPalette& p ) { return; }

	/**
	 * Returns path to an icon that represents the plugin.
	 * This icon is usualy set on button that access the
	 * plugin's GUI.
	 * @return path to an icon
	 */
	virtual QString icon() { return QString::null; }
	/**
	 * Returns a caption represents the plugin.
	 * This icon is usualy used as a caption of a button
	 * that access the plugin's GUI.
	 */
	virtual QString text() { return QString::null; }

	/**
	 * This enum shows the significance of the plugin. In reality it means that
	 * plugin with high significance must be directly on main widget, plugin with
	 * small significance must be accessible from some menu and plugin with medium
	 * significance can be (but doesn't have to) directly visible...
	 */
	enum Significance { High, Medium, Small };
	/**
	 * Returns significance of the plugin. Default is Small.
	 */
	virtual Significance significance() { return Small; }

	/**
	 * This enum represents type of GUI of the plugin.
	 */
	enum GUIType {
		GUICall,    /**< GUI is accessible only when in call. */
		GUINoCall,  /**< GUI is accessible only when not in call. */
		GUIAlways  /**< GUI is acessible always. */
	};
	/**
	 * Returns type of GUI of the plugin. This is valid only when P_HAS_GUI flag
	 * is set. Default is GUINoCall.
	 */
	virtual GUIType guiType() { return GUINoCall; }

	/**
	 * @return the PluginInfo structure of the plugin
	 */
	PluginInfo *pluginInfo() { return info; }

	static H323EndPoint *endPoint;
	static QString currentCallToken;

protected:
	PluginInfo *info;
};

class ConfigWidget: public QWidget
{
	Q_OBJECT
public:
	ConfigWidget( QWidget *parent = 0, const char *name = 0 );
	~ConfigWidget();

public slots:
	virtual void apply() = 0;
	virtual void reset() = 0;

signals:
	void changed();
};

/**
 * @short Media plugin is a common father of VideoInPlugin and AudioPlugin.
 * 
 * It defines some required methods for each of them. It is not used
 * directly.
 */
class MediaPlugin: public Plugin
{
public:
	/**
	 * Constructor.
	 * @param info PluginInfo of the plugin.
	 */
	MediaPlugin( PluginInfo *info = 0 );

	/**
	 * Add capabilities to an endpoint.
	 */
	virtual void addCapabilities() = 0;
	/**
	 * Creates a widget that will be inserted to application's
	 * main configuration dialog.
	 * @param parent parent of the widget
	 * @return widget
	 */
	virtual ConfigWidget *createCodecsConfig( QWidget *parent ) = 0;

	/**
	 * @short Get a channel that the device uses.
	 *
	 * Note that when implementing this for video, channel should
	 * be closed for playing (recording = false) since we don't know 
	 * what device use for playing. User must do it himself with
	 * the help of VideoOutPlugin.
	 *
	 * @return channel, either audio or video
	 */
	virtual PChannel *getChannel( bool recording = true ) = 0;
	/**
	 * Called when we want to use the device for call now.
	 */
	virtual void prepareForCall() = 0;
	/**
	 * Called when we want to use the device for preview now.
	 * Start the preview.
	 */
	virtual void startPreview() = 0;
	/**
	 * Called when we want to stop the preview.
	 */
	virtual void stopPreview() = 0;
};

/**
 * @short This plugin defines a device that grabs video.
 * 
 * It  sends it away (to a call participant) or sends it
 * to a local video-out device (when in preview state).
 * It must manage video capabilities.
 * 
 * Video-out device is represented by VideoOutPlugin and
 * is accessible from PluginFactory.
 */
class VideoInPlugin: public MediaPlugin {
public:
	/**
	 * Constructor.
	 * @param info PluginInfo of the plugin.
	 */
	VideoInPlugin( PluginInfo *info = 0 );
};

/**
 * @short This plugin defines a device that displays video.
 */
class VideoOutPlugin: public Plugin {
public:
	/**
	 * Constructor.
	 * @param info PluginInfo of the plugin.
	 */
	VideoOutPlugin( PluginInfo *info = 0 );

	/**
	 * Create and return the device that the plugin represents.
	 * @param w widget that should be bound with the device. We
	 * suppose that the widget is created by createVideoWidget().
	 * @return created device
	 */
	virtual PVideoOutputDevice *createVideoOutputDevice( QWidget *w ) = 0;
	/**
	 * Create a widget that will display the video pictures.
	 * @return created widget
	 */
	virtual QWidget *createVideoWidget() = 0;
	/**
	 * Delete the widget that was created by createVideoWidget().
	 */
	virtual void deleteVideoWidget( QWidget* ) = 0;
};

/**
 * @short This plugin represents audio device.
 *
 * It is mandatory for the application since
 * it is useless without it. It must manage audio capabilities.
 */
class AudioPlugin: public MediaPlugin
{
public:
	/**
	 * Constructor.
	 * @param info PluginInfo of the plugin.
	 */
	AudioPlugin( PluginInfo *info = 0 );

	/**
	 * This must be called before getChannel().
	 */
	void setBufferSize( int bufferSize )
	{ this->bufferSize = bufferSize; }

protected:
	int bufferSize;
};

/**
 * @short Prototype of the function that creates a plugin.
 * 
 * It is used in PluginInfo.
 */
typedef Plugin* createPlugin();
/**
 * @short Prototype of the function that returns created plugin.
 * 
 * It is used in PluginInfo.
 */
typedef Plugin* getPlugin();

/** This flag means that a plugin has GUI. */
const unsigned P_HAS_GUI  = 0x10000000;
/** This flag means that a plugin has a configuration dialog. */
const unsigned P_HAS_CONFIG  = 0x20000000;
/** This flag means that a plugin can be unloaded at runtime. */
const unsigned P_CAN_UNLOAD  = 0x40000000;

/**
 * @short This structure describes a plugin.
 */
struct PluginInfo
{
	/** Type of the plugin. */
	Plugin::Type	type;
	/** Flags of the plugin. See #P_CAN_UNLOAD, P_HAS_CONFIG and P_HAS_GUI. */
	int		flags;
	/** Name of the plugin. */
	const char	*name;
	/** Description of the plugin. */
	const char	*description;
	/** Author of the plugin. */
	const char	*author;
	/** Creates the plugin. */
	createPlugin	*create;
	/** When the plugin is created than get it by this function. */
	getPlugin	*get;
	/** Identification of the plugin. This must be unique for all plugins. */
	int		id;
};

/**
 * Prototype of the function that must be defined in each plugin and
 * must return the PluginInfo structure.
 */
typedef PluginInfo *GetPluginInfo();

#endif
