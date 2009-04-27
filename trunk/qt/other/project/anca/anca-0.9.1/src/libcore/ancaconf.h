#ifndef ANCACONF_H
#define ANCACONF_H

#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qmap.h>
#include <qvaluelist.h>
#include <qvaluestack.h>

#define ADMIN_GROUP "admin"

class ConfNotifier;

/**
 * @short The main configuration class for Anca.
 *
 * It is similar to KConfig from KDE or PConfig from PWLib. It is thread
 * safe. Configuration items can be stored by writeEntry() function family
 * and read by readEntry() function family.
 *
 * Configuration sections (groups) are also supported.
 *
 * User can be informed about changes of the particular configuration item
 * by installNofity().
 */
class AncaConf
{
public:
	AncaConf( const QString& globalDir, const QString& homeDir );
	~AncaConf();

	/**
	 * @name DefaultGroup
	 * Methods manipulating with the configuration within the default group.
	 */
	//@{
	QString readEntry( const QString& key, const QString& defaultValue = QString::null );
	bool readBoolEntry( const QString& key, bool defaultValue = false );
	int readIntEntry( const QString& key, int defaultValue = 0 );
	QStringList readListEntry( const QString& key, const QStringList& defaultList = QStringList() );
	QValueList<int> readIntListEntry( const QString& key );

	void writeEntry( const QString& key, const QString& value, bool notifySameMore = false );
	void writeBoolEntry( const QString& key, bool value, bool notifySameMore = false );
	void writeIntEntry( const QString& key, int value, bool notifySameMore = false );
	void writeListEntry( const QString& key, const QStringList& list, bool notifySameMore = false );
	void writeIntListEntry( const QString& key, const QValueList<int>& list, bool notifySameMore = false );
	//@}

	/**
	 * @name ConfigurationGroup
	 * Methods manipulating with the configuration within the section \p section.
	 */
	//@{
	QString readEntry( const QString& section, const QString& key, const QString& defaultValue );
	bool readBoolEntry( const QString& section, const QString& key, bool defaultValue );
	int readIntEntry( const QString& section, const QString& key, int defaultValue );
	QStringList readListEntry( const QString& section, const QString& key, const QStringList& defaultList );
	QValueList<int> readIntListEntry( const QString& section, const QString& key );

	void writeEntry( const QString& section, const QString& key, const QString& value, bool notifySameMore = false );
	void writeBoolEntry( const QString& section, const QString& key, bool value, bool notifySameMore = false );
	void writeIntEntry( const QString& section, const QString& key, int value, bool notifySameMore = false );
	void writeListEntry( const QString& section, const QString& key, const QStringList& list, bool notifySameMore = false );
	void writeIntListEntry( const QString& section, const QString& key, const QValueList<int>& list, bool notifySameMore = false );
	//@}

	/**
	 * @brief This is the default section.
	 *
	 * It is set to "default" in constructor, cannot be changed.
	 */
	const QString defaultGroup;

	/**
	 * @brief Installs notifier on key \p key.
	 *
	 * When the config item will be changed, installed signal will be emitted.
	 * Slot that catches the signal must be without parameters.
	 * Example: installNotify( "user", config, SLOT( userChanged() ) );
	 * @param key configuration key that is "observed" for changes.
	 * @param receiver object that will receive the notification
	 * @param member slot without parameters that will be called after the config item change
	 * @param group group group in which context the key is valid
	 */
	void installNotify( const QString& key, const QObject *receiver, const char *member, const QString& group = "default" );

	/**
	 * @brief Removes notifier on key \p key
	 *
	 * This removes notifier that was installed by installNotify(). Parameter are the same.
	 */
	void removeNotify( const QString& key, const QObject *receiver, const char *member, const QString& group = "default" );

protected:
	bool load( const QString& fileName, bool ignoreAdminGroup = false );
	bool save( const QString& fileName, bool ignoreAdminGroup = false );

	/** Save configuration to anca.ini in home directory. */
	void saveToHome();

	typedef QMap<QString, QString> GroupConfig;
	typedef QMap<QString, GroupConfig> Config;

	Config config;

	QValueList<int> stringToIntList( QStringList list );
	QStringList intToStringList( const QValueList<int>& list );

	typedef QPair<QString, QString> KeyPair;
	typedef QMap<KeyPair, ConfNotifier*> NotifierMap;
	NotifierMap notifierMap;

	QString globalDir;
	QString homeDir;

	QValueStack<QString> groupStack;
};
extern AncaConf *ancaConf;


class ConfNotifier: public QObject
{
	Q_OBJECT
public:
	void notify() { emit changed(); }

signals:
	void changed();
};


#endif
