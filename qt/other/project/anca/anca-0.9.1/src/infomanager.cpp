//
// C++ Implementation: infocontrol
//
// Description:
//
//
// Author: Tom Potrusil <tom@matfyz.cz>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "infomanager.h"
#include "ancainfo.h"
#include "stdinfo.h"
#include "guimanager.h"

#include <qdatetime.h>
#include <qlabel.h>

InfoManager *infoManager = 0;

InfoManager::InfoManager()
{
	infoManager = this;

	ancaInfo->install( LOGIN );
	ancaInfo->install( ADDRESS );
	ancaInfo->install( TIME );
	ancaInfo->install( DATE );

	startTimer( 500 );

	ancaInfo->install( VG_STATE, VG_IDLE );
	ancaInfo->install( CALL_STATE, CALL_IDLE, true );
	ancaInfo->install( GK_STATE, GK_IDLE );

	// install call duration notification
	ancaInfo->install( CALL_DURATION );
	ancaInfo->install( PARTY_NAME );
	ancaInfo->install( PARTY_HOST );
	ancaInfo->install( PARTY_PICTURE, QString::null, true );

	ancaInfo->install( ADDRESS_CALLING );
	ancaInfo->install( ADDRESS_TO_CALL, QString::null, true );
	ancaInfo->install( ADDRESS_TO_CALL_NOW, QString::null, true );
	ancaInfo->install( CALLER_ADDRESS );

	ancaInfo->install( MESSAGE, QString::null, true );

	ancaInfo->install( HTTP_PUSH, QString::null, true );
	ancaInfo->install( HTTP_CLOSE, QString::null, true );
}


InfoManager::~InfoManager()
{
	infoManager = 0;
}

void InfoManager::init()
{
	// catch some basic info
	ancaInfo->connectNotify( VG_STATE, this, SLOT( videoGrabberStateChanged( const QVariant& ) ), false );
	ancaInfo->connectNotify( CALL_STATE, this, SLOT( callStateChanged( const QVariant& ) ), false );
	ancaInfo->connectNotify( GK_STATE, this, SLOT( gkStateChanged( const QVariant& ) ), false );
	ancaInfo->connectNotify( MESSAGE, this, SLOT( generalMessage( const QVariant& ) ), false );
}

void InfoManager::timerEvent( QTimerEvent * )
{
	ancaInfo->set( TIME, QTime::currentTime().toString() );
	ancaInfo->set( DATE, QDate::currentDate().toString(Qt::LocalDate) );
}

void InfoManager::videoGrabberStateChanged( const QVariant& to )
{
	Q_ASSERT( guiManager );

	switch( (VG_StateType)to.toInt() ) {
		case VG_OPENING:
			emit message( "Opening video grabber...");
			break;
		case VG_OPENED:
			emit message( "Video grabber opened");
			break;
		case VG_CLOSED:
			emit message( "Video grabber closed");
			break;
		case VG_IDLE:
			guiManager->hidePreview();
			break;
	}
}

void InfoManager::callStateChanged( const QVariant& to )
{
	Q_ASSERT( guiManager );

	switch( (CALL_StateType)to.toInt() ) {
		case CALL_CLOSED:
			guiManager->callCleared();
			break;
		case CALL_STARTED:
			guiManager->callStarted();
			break;
		case CALL_CALLING:
			guiManager->callingStarted();
			break;
		case CALL_HELD:
			guiManager->callHeld();
			break;
		default:
			break;
	}
}

void InfoManager::gkStateChanged( const QVariant& to )
{
	switch( (GK_StateType)to.toInt() ) {
		case GK_REGISTERED:
			emit message( "Registered with gatekeeper." );
			break;
		case GK_NOT_REGISTERED:
			emit message( "Not registered with gatekeeper." );
			break;
		case GK_REGISTERING:
			emit message( "Trying to register with gatekeeper..." );
			break;
		default:
			break;
	}
}
void InfoManager::generalMessage( const QVariant& to )
{
	emit message( to.toString() );
}
