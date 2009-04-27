//
// C++ Implementation: ancainfo
//
// Description:
//
//
// Author: Tom Potrusil <tom@matfyz.cz>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "ancainfo.h"

#include <ptlib.h>
#include <qapplication.h>

AncaInfo *ancaInfo = 0;

AncaInfoItem::AncaInfoItem( const QVariant& value, bool notifySameMore ) 
: value( value ), notifySameMore(notifySameMore)
{
}

AncaInfoItem::AncaInfoItem()
{
}

AncaInfoItem::AncaInfoItem( const AncaInfoItem& item ) 
: value( item.value ), notifySameMore( item.notifySameMore )
{
}

void AncaInfoItem::set( const QVariant& to )
{
	if( !notifySameMore && (value == to) )
		return;
	value = to;

	PTRACE(10, "AncaInfoItem::set: value changed to: " << to.toString().latin1() );

	emitChanged();
}

const QVariant& AncaInfoItem::get()
{
	return value;
}

AncaInfoItem &AncaInfoItem::operator=( const AncaInfoItem& item )
{
	value = item.value;
	notifySameMore = item.notifySameMore;
	return *this;
}

#define AncaInfoItemEventType (QEvent::User + 100)

void AncaInfoItem::emitChanged()
{
	if (PThread::Current()->GetThreadName() == "anca")
		emit changed(value);
	else
		//emit notification in the main thread
		qApp->postEvent( this, new QCustomEvent( AncaInfoItemEventType ) );
}

void AncaInfoItem::customEvent( QCustomEvent *e )
{
	//now we are in the main thread
	if( e->type() == AncaInfoItemEventType )
		emit changed(value);
	else
		QObject::customEvent(e);
}

AncaInfo::AncaInfo()
{
	ancaInfo = this;
}

AncaInfo::~AncaInfo()
{}

void AncaInfo::install( const QString& key, const QVariant& initialValue, bool notifySameMore )
{
	items[ key ] = AncaInfoItem( initialValue, notifySameMore );
}

void AncaInfo::connectNotify( const QString& key, const QObject * receiver, const char * member, bool notifyImmediately )
{
	if ( !items.contains( key ) ) {
		PTRACE(1, "Cannot set up notify to an item (" << key.latin1() << ") that is not installed");
		return ;
	}

	AncaInfoItem *item = &items[ key ];
	connect( item, SIGNAL( changed( const QVariant& ) ), receiver, member );
	
	if( notifyImmediately && !item->get().isNull() )
		item->emitChanged();
}

void AncaInfo::disconnectNotify( const QString& key, const QObject * receiver, const char * member )
{
	if ( !items.contains( key ) )
		return ;

	AncaInfoItem *item = &items[ key ];
	disconnect( item, SIGNAL( changed( const QVariant& ) ), receiver, member );
}

QString AncaInfo::get( const QString& key )
{
	return items[ key ].get().toString();
}

int AncaInfo::getInt( const QString& key )
{
	return items[ key ].get().toInt();
}

bool AncaInfo::getBool( const QString& key )
{
	return items[ key ].get().toBool();
}

const QVariant& AncaInfo::getVariant( const QString& key )
{
	return items[ key ].get();
}

void AncaInfo::set( const QString& key, const QString& value )
{
	items[ key ].set( value );
}

void AncaInfo::setInt( const QString& key, int value )
{
	items[ key ].set( value );
}

void AncaInfo::setBool( const QString& key, bool value )
{
	items[ key ].set( value );
}

void AncaInfo::setVariant( const QString& key, const QVariant& value )
{
	items[ key ].set( value );
}
