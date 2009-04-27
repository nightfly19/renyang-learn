#include "anca.h"

#include "ancaconf.h"
#include "endpoint.h"
#include "slavethread.h"
#include "version.h"
#include "confdefines.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

Anca *anca = 0;

// ***********************************************************************

Anca::Anca() : PProcess( "vt", "anca", ANCA_VER_MAJOR, ANCA_VER_MINOR, ANCA_VER_STATUS, ANCA_VER_BUILD )
{
	anca = this;
}


// **********************************************************************

Anca::~Anca()
{
	if ( endPoint ) {
		PTRACE(3, "Deleting EndPoint");
		delete endPoint;
	}

	anca = 0;
}

// **********************************************************************

bool Anca::init( PConfigArgs& args )
{
	PTRACE(3, "Creating EndPoint");
	new EndPoint(args);

#ifdef ENTERPRISE
	endPoint->fetchLDAP();

	// Create H323EndPoint and initialize it
	if ( !endPoint->init() )
		return false;

	// register with gatekeeper directly
	if( !endPoint->registerGk() )
		return false;

#else
	slaveThread->doInstruction( SlaveThread::InitEndPoint );

	// register with gatekeeper in slave thread
	registerGk();
#endif

	// Call the user...
	if( args.GetCount() > 0 )
		slaveThread->doInstruction( SlaveThread::MakeInitialCall );


	ancaConf->installNotify( REGISTER_WITH_GK, this, SLOT( registerGk() ) );

	//Call diversion notifiers
	ancaConf->installNotify( USE_GK_DIVERSION, this, SLOT( setDiversion() ) );

	return true;
}

// ***********************************************************************

void Anca::shutdown()
{
	if ( endPoint ) {
		endPoint->ClearAllCalls ( H323Connection::EndedByLocalUser, TRUE );
	}
}

// ***********************************************************************

void Anca::registerGk()
{
	// Let the slave thread registers with gatekeeper
	slaveThread->doInstruction( SlaveThread::RegisterWithGk );
}

// ***********************************************************************

void Anca::setDiversion()
{
#ifndef NO_CALL_DIVERSIONS
	slaveThread->doInstruction( SlaveThread::SetDiversion );
#endif
}

// ***********************************************************************

bool Anca::transferCall( const PString& remoteParty )
{
	if( endPoint->transferHeldCall() ) return true;
	if( endPoint->transferCall( remoteParty ) ) return true;

	return false;
}

// ***********************************************************************

bool Anca::call( const PString& remoteParty )
{
	return endPoint->makeCall( remoteParty );
}

// ***********************************************************************

void Anca::hangUp()
{
	endPoint->hangUpCurrent();
}

// ***********************************************************************

bool Anca::holdRetrieveCall()
{
	return endPoint->holdRetrieveCall();
}
