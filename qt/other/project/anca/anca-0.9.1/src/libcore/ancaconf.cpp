#include "ancaconf.h"

#include <qfile.h>
#include <qdir.h>

#include <ptlib.h>

AncaConf *ancaConf = 0;

// it is important that the mutex is recursive!!!
PMutex mutex;

AncaConf::AncaConf( const QString& globalDir, const QString& homeDir )
	: defaultGroup( "default" ), globalDir(globalDir), homeDir(homeDir)
{
	PAssert( !ancaConf, "AncaConf already created!" );
	ancaConf = this;

	load( globalDir + "/anca.ini" );
	load( QDir::homeDirPath() + "/" + homeDir + "/anca.ini", true );

//	PTrace::SetLevel( readIntEntry( "tracingLevel", 1 ) );

	/* print initial configuration */
#ifdef PTRACING
	if( PTrace::CanTrace(6) ) {
		PString configuration("Initial configuration:\n");

		/* iterate all config groups */
		for( Config::iterator it = config.begin(); it != config.end(); ++it ) {
			configuration += ("[" + PString(it.key().latin1()) + "]\n");

			/* iterate all items in group */
			GroupConfig group = it.data();
			for( GroupConfig::iterator it2 = group.begin(); it2 != group.end(); ++it2 )
				configuration += ("\t" + PString(it2.key().latin1()) + "\t= " + PString(it2.data().latin1()) + "\n");
		}
		PTRACE(10, configuration);
	}
#endif
}

AncaConf::~AncaConf()
{
	saveToHome();

	for( NotifierMap::Iterator it = notifierMap.begin(); it != notifierMap.end(); ++it ) {
		ConfNotifier *c = *it;
		delete c;
	}
}

bool AncaConf::load( const QString& fileName, bool ignoreAdminGroup )
{
	PTRACE(6, "Loading configuration from " << fileName.latin1() );
	/*
	QFile file( fileName );
	if( !file.open( IO_ReadOnly ) ) {
		PTRACE( 1, "Cannot load configuration " + fileName );
		return false;
	}
	QTextStream stream( &file );

	QString group = "default";

	while( !stream.atEnd() ) {
		QString line = stream.readLine();

		// read another line if line is empty or line starts with '#'
		if( line.isEmpty() || line[0] == '#' ) continue;

		/ * group definition line - should be surrounded by '[' and ']'
		 * this implementation returns a substring starting with the
		 * second character and ending with a character before the
		 * first ']'
		 *\/
		if( line[0] == '[' ) {
			group = line.mid(1, line.find(']') - 1);
			continue;
		}

		if( !ignoreAdminGroup || group != ADMIN_GROUP ) {
			QString key = line.section( ' ', 0, 0 );
			QString data = line.section( ' ', 1 );
			config[group][key] = data;
		}
	}

	file.close();
	*/

	// load it using PConfig
	PConfig c( fileName.latin1(), defaultGroup.latin1() );
	PStringList sections = c.GetSections();
	PStringList keys;
	for( int i = 0; i < sections.GetSize(); i++ ) {
		PString section = sections[i];
		if( ignoreAdminGroup && (section == ADMIN_GROUP) ) continue;
		
		keys = c.GetKeys(section);
		for( int j = 0; j < keys.GetSize(); j++ ) {
			PString key = keys[j];
			config[(const char *)section][(const char *)key] = (const char *)c.GetString(section, key, PString());
		}
	}

	return true;
}

bool AncaConf::save( const QString& fileName, bool ignoreAdminGroup )
{
	PTRACE(6, "Saving configuration to " << fileName.latin1() );

	/*
	QFile file( fileName );
	if( !file.open( IO_WriteOnly ) ) {
		PError << "Cannot save configuration to " << fileName.latin1() << endl;
		return false;
	}
	QTextStream stream( &file );

	// iterate all config groups
	for( Config::iterator it = config.begin(); it != config.end(); ++it ) {
		if( it.key() == ADMIN_GROUP && ignoreAdminGroup ) continue;

		stream << "[" << it.key() << "]\n";

		// iterate all items in group
		GroupConfig group = it.data();
		for( GroupConfig::iterator it2 = group.begin(); it2 != group.end(); ++it2 )
			stream << it2.key() << " " << it2.data() << "\n";

		stream << "\n";
	}

	file.close();
	*/

	/* save it using PConfig */
	PConfig c( PFilePath(fileName.latin1()), defaultGroup.latin1() );

	for( Config::iterator it = config.begin(); it != config.end(); ++it ) {
		if( it.key() == ADMIN_GROUP && ignoreAdminGroup ) continue;

		// iterate all items in group
		GroupConfig group = it.data();
		for( GroupConfig::iterator it2 = group.begin(); it2 != group.end(); ++it2 )
			c.SetString( it.key().latin1(), it2.key().latin1(), it2.data().latin1() );
	}
	
	return true;

}

QString AncaConf::readEntry( const QString& key, const QString& defaultValue )
{
	return readEntry( QString::null, key, defaultValue );
}

bool AncaConf::readBoolEntry( const QString& key, bool defaultValue )
{
	return readBoolEntry( QString::null, key, defaultValue );
}

int AncaConf::readIntEntry( const QString& key, int defaultValue )
{
	return readIntEntry( QString::null, key, defaultValue );
}

QStringList AncaConf::readListEntry( const QString& key, const QStringList& defaultList )
{
	return readListEntry( QString::null, key, defaultList );
}

QValueList<int> AncaConf::readIntListEntry( const QString& key )
{
	return readIntListEntry( QString::null, key );
}

void AncaConf::writeEntry( const QString& key, const QString& value, bool notifySameMore )
{
	writeEntry( QString::null, key, value, notifySameMore );
}

void AncaConf::writeBoolEntry( const QString& key, bool value, bool notifySameMore )
{
	writeBoolEntry( QString::null, key, value, notifySameMore );
}

void AncaConf::writeIntEntry( const QString& key, int value, bool notifySameMore )
{
	writeIntEntry( QString::null, key, value, notifySameMore );
}

void AncaConf::writeListEntry( const QString& key, const QStringList& list, bool notifySameMore )
{
	writeListEntry( QString::null, key, list, notifySameMore );
}

void AncaConf::writeIntListEntry( const QString& key, const QValueList<int>& list, bool notifySameMore )
{
	writeIntListEntry( QString::null, key, list, notifySameMore );
}

//----------------- group-can-be-changed methods -------------------
QString AncaConf::readEntry( const QString& group_, const QString& key, const QString& defaultValue )
{
	PWaitAndSignal lock(mutex);

	QString group = group_;
	if( group.isEmpty() ) group = defaultGroup;

	if ( !config.contains( group ) ) {
		return defaultValue;
	}

	GroupConfig& confGroup = config[ group ];
	if ( !confGroup.contains( key ) ) {
		return defaultValue;
	}

	QString& value = confGroup[ key ];

	return value;
}

bool AncaConf::readBoolEntry( const QString& group, const QString& key, bool defaultValue )
{
	return readEntry( group, key, QString::number( defaultValue ) ).toInt();
}

int AncaConf::readIntEntry( const QString& group, const QString& key, int defaultValue )
{
	return readEntry( group, key, QString::number( defaultValue ) ).toInt();
}

QStringList AncaConf::readListEntry( const QString& group, const QString& key, const QStringList& defaultList )
{
	QString value = readEntry( group, key, QString::null );
	return value.isNull() ? defaultList : QStringList::split( ',', value );
}

QValueList<int> AncaConf::stringToIntList( QStringList list )
{
	QValueList<int> intList;
	for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it )
		intList.append( ( *it ).toInt() );

	return intList;
}

QValueList<int> AncaConf::readIntListEntry( const QString& group, const QString& key )
{
	QString value = readEntry( group, key, QString::null );
	return value.isNull() ? QValueList<int>() : stringToIntList( QStringList::split( ',', value ) );
}

void AncaConf::writeEntry( const QString& group_, const QString& key, const QString& value, bool notifySameMore )
{
	mutex.Wait();

	QString group = group_;
	if( group.isEmpty() ) group = defaultGroup;

	//create currentGroup if it doesn't exist
	if ( !config.contains( group ) )
		config.insert( group, GroupConfig() );

	GroupConfig& configGroup = config[ group ];

	//do not continue if the value has not changed
	if( !notifySameMore && configGroup[key] == value ) goto out;
	configGroup[key] = value;

	//notify listeners
	if( notifierMap.contains(KeyPair(key, group)) ) {
		//unlock when notifying
		mutex.Signal();
		notifierMap[KeyPair(key, group)]->notify();
		mutex.Wait();
	}

	saveToHome();
out:
	mutex.Signal();
}

void AncaConf::writeBoolEntry( const QString& group, const QString& key, bool value, bool notifySameMore )
{
	writeEntry( group, key, QString::number( value ), notifySameMore );
}

void AncaConf::writeIntEntry( const QString& group, const QString& key, int value, bool notifySameMore )
{
	writeEntry( group, key, QString::number( value ), notifySameMore );
}

void AncaConf::writeListEntry( const QString& group, const QString& key, const QStringList& list, bool notifySameMore )
{
	writeEntry( group, key, list.join( "," ), notifySameMore );
}

QStringList AncaConf::intToStringList( const QValueList<int>& list )
{
	QStringList stringList;
	for ( QValueList<int>::const_iterator it = list.begin(); it != list.end(); ++it )
		stringList.append( QString::number( *it ) );

	return stringList;
}

void AncaConf::writeIntListEntry( const QString& group, const QString& key, const QValueList<int>& list, bool notifySameMore )
{
	QStringList stringList = intToStringList( list );
	writeListEntry( group, key, stringList, notifySameMore );
}
// -----------------------------------------------------------------

void AncaConf::installNotify( const QString& key, const QObject *receiver, const char *member, const QString& group )
{
	ConfNotifier *notifier = 0;
	if( notifierMap.contains(KeyPair(key, group)) )
		notifier = notifierMap[KeyPair(key, group)];
	else
		notifier = notifierMap[KeyPair(key, group)] = new ConfNotifier();

	QObject::connect( notifier, SIGNAL( changed() ), receiver, member );
}

void AncaConf::removeNotify( const QString& key, const QObject *receiver, const char *member, const QString& group )
{
	if( !notifierMap.contains(KeyPair(key, group)) ) return;

	ConfNotifier *notifier = notifierMap[KeyPair(key, group)];

	QObject::disconnect( notifier, SIGNAL( changed() ), receiver, member );
}

void AncaConf::saveToHome()
{
	// create home directory if it doesn't exist
	QDir home(QDir::home());
	if( !home.cd(homeDir) )
		home.mkdir(homeDir);

	save( home.path() + "/anca.ini", true );
}

