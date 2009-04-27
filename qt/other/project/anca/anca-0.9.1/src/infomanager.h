//
// C++ Interface: infomanager
//
// Description: 
//
//
// Author: Tom Potrusil <tom@matfyz.cz>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef INFOMANAGER_H
#define INFOMANAGER_H

#include <qobject.h>

/**
 * @author Tom Potrusil
 * @short This class manages info events from AncaInfo that are
 * used within the application.
 */
class InfoManager: public QObject
{
	Q_OBJECT
public:
	/**
	 * Create managed events.
	 */
	InfoManager();

	~InfoManager();

	/**
	 * Connect notifications of the managed events.
	 */
	void init();

signals:
	/** 
	 * @short When emitted the message \p str whould be shown to a user.
	 *
	 * Currently it is just connected to AncaForm::message().
	 */
	void message( const QString& str );

public slots:
	void videoGrabberStateChanged( const QVariant& to );
	void gkStateChanged( const QVariant& to );
	void callStateChanged( const QVariant& to );
	void generalMessage( const QVariant& to );
	
protected:
	void timerEvent( QTimerEvent * );
};
extern InfoManager *infoManager;

#endif
