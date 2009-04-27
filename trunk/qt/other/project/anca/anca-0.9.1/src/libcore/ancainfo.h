//
// C++ Interface: ancainfo
//
// Description:
//
//
// Author: Tom Potrusil <tom@matfyz.cz>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef ANCAINFO_H
#define ANCAINFO_H

#include <qobject.h>
#include <qvariant.h>

class AncaInfoItem;

/**
 * @short AncaInfo class is used to inform an application about temporal changes.
 *
 * It is thread safe. Qt signal/slot mechanism + posting events is used. We call
 * units that represents the information "info items". These items must be installed
 * by install() method. That item can be changed or read by getXX or setXX functions.
 * Notifiers (similar to those in AncaConf) can be installed to observe the changes:
 * connectNotify() and disconnectNotify().
 */
class AncaInfo: public QObject
{
	Q_OBJECT
public:
	AncaInfo();
	~AncaInfo();

	/**
	 * Installs new info item. This must be done before changing it by setXX functions.
	 * 
	 * @param key info item will be represented be the key
	 * @param initialValue to what value the info item will be initialized
	 * @param notifySameMore notifiers will be called even when the value set is the same as the old one.
	 */
	void install( const QString& key, const QVariant& initialValue = QString::null, bool notifySameMore = false);

	/**
	 * Connects new notifier to the info item represented by key. This notifier
	 * is a Qt signal.
	 * 
	 * @param key key of info item
	 * @param receiver object that will receive the notifier (Qt signal)
	 * @param member Qt slot that will be called on notify
	 * @param notifyImmediately notify the receiver immediately (do not wait for the set() call)
	 */
	void connectNotify( const QString& key, const QObject * receiver, const char * member, bool notifyImmediately = true );
	
	/**
	 * Disconnects notifier that was connected be connectNotify(). Parameters are the same.
	 */
	void disconnectNotify( const QString& key, const QObject * receiver, const char * member );

	/**
	 * Get the value of the info item.
	 * @param key requested info item is represented be the key
	 * @return value of the info item
	 */
	QString get( const QString& key );
	/**
	 * The same as above.
	 */
	int getInt( const QString& key );
	/**
	 * The same as above.
	 */
	bool getBool( const QString& key );
	/**
	 * The same as above.
	 */
	const QVariant& getVariant( const QString& key );

public slots:
	/**
	 * Set the value of the info item. Notifiers may be emitted.
	 * @param key requested info item is represented be the key
	 * @param value new value of the info item.
	 */
	void set( const QString& key, const QString& value );
	/**
	 * The same as above.
	 */
	void setInt( const QString& key, int value );
	/**
	 * The same as above.
	 */
	void setBool( const QString& key, bool value );
	/**
	 * The same as above.
	 */
	void setVariant( const QString& key, const QVariant& value );

protected:	
	QMap<QString, AncaInfoItem> items;
};
extern AncaInfo *ancaInfo;


class AncaInfoItem: public QObject
{
	friend class AncaInfo;
	Q_OBJECT
public:
	AncaInfoItem( const QVariant& value, bool notifySameMore );
	AncaInfoItem();
	AncaInfoItem( const AncaInfoItem& item );

	void set( const QVariant& to );
	const QVariant& get();
	
	AncaInfoItem &operator=( const AncaInfoItem& );
	
protected:
	/**
	 * Emit signal that the value has changed. The signal is emitted in the
	 * context of the main (GUI) thread.
	 */
	void emitChanged();

	/**
	 * With help of this callback emitChanged() can emit signals in the
	 * context of the main thread.
	 */
	void customEvent( QCustomEvent * );

signals:
	void changed( const QVariant& to );
	
protected:
	QVariant value;
	bool notifySameMore;
};


#endif
